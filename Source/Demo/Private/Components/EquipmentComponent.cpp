// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/ItemTypes.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Character.h"
#include "Items/Item.h"
#include "PlayerController/DemoPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ItemActionDispatcher.h"

UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();

    InitEquipmentComponent();
    BindToItemActionDispatcher();
}

bool UEquipmentComponent::EquipItem(const FItemSlot& InSlot)
{
    // Validate and get data
    FEquipmentValidationResult ValidationResult = EquipItem_Validate(InSlot);
    if (!ValidationResult.bIsValid)
    {
        return false; // Log in EquipItem_Validate()
    }

    // Handle conflicts
    if (!EquipItem_HandleConflicts(ValidationResult))
    {
        return false;
    }

    // Spawn
    AItem* SpawnedItem = EquipItem_SpawnItem(InSlot);
    if (!IsValid(SpawnedItem))
    {
        DemoLOG_F(LogEquipment, Warning, TEXT("Failed to spawn item."));
        return false;
    }

    // Attach
    if (!AttachActor(SpawnedItem, ValidationResult.EquipmentType))
    {
        DemoLOG_F(LogEquipment, Warning, TEXT("Failed to attach item."));
        SpawnedItem->Destroy();
        return false;
    }

    *(ValidationResult.EquippedItemPtr) = SpawnedItem;

    EquipItem_PostProcess(ValidationResult);

    UE_LOG(LogEquipment, Verbose, TEXT("Equipped - %s"), *InSlot.RowHandle.RowName.ToString());
    return true;
}

bool UEquipmentComponent::UnequipItem(const FGameplayTag EquipmentType)
{
    if (const UStateManagerComponent* StateManager = GetStateManager())
    {
        if (!StateManager->CanChangeEquipment())
        {
            return false;
        }
    }

    AItem* EquippedItem = GetEquippedItem(EquipmentType);
    if (!EquippedItem)
    {
        DemoLOG_F(LogEquipment, Display, TEXT("Not equipped %s."), *EquipmentType.ToString());
        return false;
    }

    // Add to inventory if possible
    if (!UnequipItem_AddToInventory(EquippedItem->GetItemSlot()))
    {
        return false; // Log in UnequipItem_AddToInventory()
    }

    // Destroy
    if (!EquippedItem->Destroy())
    {
        // @TODO - Handle failure, already added to inventory so it should be destroyed
        DemoLOG_F(LogEquipment, Fatal, TEXT("Failed to destroy item, but already added to inventory."));
        return false;
    }

    EquippedItems[EquipmentType] = nullptr;

    UnequipItem_PostProcess(EquipmentType);

    UE_LOG(LogEquipment, Verbose, TEXT("Unequipped - %s"), *EquipmentType.ToString());
    return true;
}

bool UEquipmentComponent::UnequipAndDropItem(const FGameplayTag EquipmentType)
{
    if (const UStateManagerComponent* StateManager = GetStateManager())
    {
        if (!StateManager->CanChangeEquipment())
        {
            return false;
        }
    }

    AItem* EquippedItem = GetEquippedItem(EquipmentType);
    if (!EquippedItem)
    {
        DemoLOG_F(LogEquipment, Display, TEXT("Not equipped %s."), *EquipmentType.ToString());
        return false;
    }

    // Detach
    EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // Drop
    bool bDropped = EquippedItem->Drop(GetOwner());
    if (!bDropped) // Unlikely, but just in case
    {
        DemoLOG_F(LogEquipment, Error, TEXT("Failed to drop item, re-attaching."));
        AttachActor(EquippedItem, EquipmentType);
        return false;
    }

    EquippedItems[EquipmentType] = nullptr;

    UnequipItem_PostProcess(EquipmentType);

    UE_LOG(LogEquipment, Verbose, TEXT("Unequipped - %s"), *EquipmentType.ToString());
    return true;
}

void UEquipmentComponent::DestroyAllEquippedItems()
{
    for (TPair<FGameplayTag, TObjectPtr<AItem>>& Pair : EquippedItems)
    {
        TObjectPtr<AItem>& EquippedItem = Pair.Value;
        if (IsValid(EquippedItem))
        {
            EquippedItem->Destroy();
            EquippedItem = nullptr;
        }
    }
}

void UEquipmentComponent::InitEquipmentComponent()
{
    for (const FGameplayTag& EquipmentType : DemoItemTypes::GetEquipmentTypes())
    {
        EquippedItems.Add(EquipmentType, nullptr);
    }

    checkf(EquippedItems.Num() == DemoItemTypes::GetEquipmentTypes().Num(), TEXT("EquippedItems should have all EquipmentTypes."));

    // @TODO - Config file or data table?
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Weapon, TEXT("MeleeHandSocket"));
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Armor_Shield, TEXT("ShieldHandSocket"));
}

void UEquipmentComponent::BindToItemActionDispatcher()
{
    if (APawn* Pawn = Cast<APawn>(GetOwner()))
    {
        if (ADemoPlayerController* DemoPlayerController = Pawn->GetController<ADemoPlayerController>())
        {
            if (UItemActionDispatcher* ItemActionDispatcher = DemoPlayerController->GetItemActionDispatcher())
            {
                ItemActionDispatcher->OnUnequipItemRequested.BindUObject(this, &ThisClass::UnequipItem);
                ItemActionDispatcher->OnUnequipAndDropItemRequested.BindUObject(this, &ThisClass::UnequipAndDropItem);
            }
        }
    }
}

FEquipmentValidationResult UEquipmentComponent::EquipItem_Validate(const FItemSlot& InSlot)
{
    if (const UStateManagerComponent* StateManager = GetStateManager())
    {
        if (!StateManager->CanChangeEquipment())
        {
            return {};
        }
    }

    if (!InSlot.IsValid())
    {
        DemoLOG_F(LogEquipment, Warning, TEXT("Slot is not valid."));
        return {};
    }

    const FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UEquipmentComponent::EquipItem_Validate"));
    if (!ItemData)
    {
        return {}; // Log in GetRow()
    }

    FGameplayTag ItemType = ItemData->ItemType;
    FGameplayTag EquipmentType = DemoItemTypes::GetEquipmentType(ItemData->ItemType);
    if (!EquipmentType.IsValid())
    {
        DemoLOG_F(LogEquipment, Error, TEXT("Item is not equippable."));
        return {};
    }

    // Tag validation
    TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(EquipmentType);
    if (!EquippedItemPtr)
    {
        DemoLOG_F(LogEquipment, Error, TEXT("EquipmentType is not valid."));
        return {};
    }

    FEquipmentValidationResult Result;
    Result.bIsValid = true;
    Result.ItemType = ItemType;
    Result.EquipmentType = EquipmentType;
    Result.EquippedItemPtr = EquippedItemPtr;

    return Result;
}

bool UEquipmentComponent::EquipItem_HandleConflicts(const FEquipmentValidationResult& ValidationResult)
{
    // Unequip already equipped item of the same type
    const AItem* EquippedItem = *(ValidationResult.EquippedItemPtr);
    if (EquippedItem)
    {
        if (!UnequipItem(ValidationResult.EquipmentType))
        {
            DemoLOG_F(LogEquipment, Warning, TEXT("Failed to unequip."));
            return false;
        }
    }

    // Unequip shield when two-handing
    if (ValidationResult.ItemType.MatchesTag(DemoGameplayTags::Item_Weapon_Melee_TwoHanded))
    {
        if (GetEquippedItem(DemoGameplayTags::Item_Armor_Shield))
        {
            if (!UnequipItem(DemoGameplayTags::Item_Armor_Shield))
            {
                DemoLOG_F(LogEquipment, Warning, TEXT("Failed to unequip shield."));
                return false;
            }
        }
    }

    // Can't equip shield when two-handing
    if (ValidationResult.EquipmentType == DemoGameplayTags::Item_Armor_Shield)
    {
        if (CurrentWeaponType.MatchesTag(DemoGameplayTags::Item_Weapon_Melee_TwoHanded))
        {
            // @misc - In-game notification?
            DemoLOG_F(LogEquipment, Display, TEXT("Can't equip shield when two-handing."));
            return false;
        }
    }

    return true; // Can equip
}

AItem* UEquipmentComponent::EquipItem_SpawnItem(const FItemSlot& InSlot) const
{
    UWorld* World = GetWorld();
    APawn* OwnerPawn = GetOwner<APawn>();
    if (!World || !OwnerPawn)
    {
        return nullptr;
    }

    // Spawn
    FTransform SpawnTransform = FTransform::Identity;
    AItem* SpawnedItem = AItem::SpawnItem(World, InSlot, SpawnTransform, true, OwnerPawn, OwnerPawn);
    if (!SpawnedItem)
    {
        return nullptr;
    }

    // Destroy without mesh -> change if there exists no-mesh-EquipmentType
    if (!SpawnedItem->IsMeshAssetValid())
    {
        DemoLOG_F(LogEquipment, Error, TEXT("Spawned item has no mesh."));
        SpawnedItem->Destroy();
        return nullptr;
    }

    return SpawnedItem;
}

bool UEquipmentComponent::AttachActor(AActor* ActorToAttach, FGameplayTag EquipmentType) const
{
    ACharacter* OwnerCharacter = GetOwner<ACharacter>();
    USkeletalMeshComponent* OwnerMesh = OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr;
    if (!ActorToAttach || !OwnerMesh)
    {
        return false;
    }

    // @TODO - Move socketName to FItemData?
    // Socket name
    const FName* SocketNamePtr = EquipDefaultSocketNames.Find(EquipmentType);
    if (!SocketNamePtr)
    {
        DemoLOG_F(LogEquipment, Error, TEXT("No default socket name for %s."), *EquipmentType.ToString());
        return false;
    }
    const FName SocketName = *SocketNamePtr;

#if WITH_EDITOR
    if (!OwnerMesh->DoesSocketExist(SocketName))
    {
        DemoLOG_F(LogEquipment, Warning, TEXT("Socket %s does not exist."), *SocketName.ToString());
        return false;
    }
#endif // WITH_EDITOR

    const auto AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
    if (!ActorToAttach->AttachToComponent(OwnerMesh, AttachRules, SocketName))
    {
        DemoLOG_F(LogEquipment, Warning, TEXT("Failed to attach %s to %s."), *ActorToAttach->GetName(), *SocketName.ToString());
        return false;
    }

    return true;
}

void UEquipmentComponent::EquipItem_PostProcess(const FEquipmentValidationResult& ValidationResult)
{
    if (ValidationResult.EquipmentType == DemoGameplayTags::Item_Weapon)
    {
        // Add attack collision definition if it's a weapon
        const AItem* EquippedWeapon = *(ValidationResult.EquippedItemPtr);
        const FItemSlot& WeaponSlot = EquippedWeapon->GetItemSlot();
        const FWeaponData* WeaponData = WeaponSlot.RowHandle.GetRow<FWeaponData>(TEXT("UEquipmentComponent::EquipItem_PostProcess"));

        if (WeaponData)
        {
            CurrentWeaponType = ValidationResult.ItemType;
            OnWeaponChanged.Broadcast(WeaponData);
        }
    }

    // OnEquipped: Update UI, stats, etc.
    OnEquipmentChanged.Broadcast(ValidationResult.EquipmentType);

    if (EquipSound)
    {
        UGameplayStatics::PlaySound2D(this, EquipSound);
    }

    // Register active skills
}

bool UEquipmentComponent::UnequipItem_AddToInventory(const FItemSlot& InSlot)
{
    UInventoryComponent* InventoryComponent = GetInventoryComponent();
    if (!InventoryComponent)
    {
        DemoLOG_F(LogEquipment, Error, TEXT("Can't unequip without InventoryComponent."));
        return false;
    }

    FItemActionRequest Request;
    Request.Slot = InSlot;
    Request.Quantity = 1;

    if (InventoryComponent->AddItem(Request) <= 0)
    {
        // @misc - In-game notification?
        DemoLOG_F(LogEquipment, Warning, TEXT("Failed to add item to inventory."));
        return false;
    }

    return true;
}

void UEquipmentComponent::UnequipItem_PostProcess(FGameplayTag EquipmentType)
{
    if (EquipmentType == DemoGameplayTags::Item_Weapon)
    {
        CurrentWeaponType = DemoGameplayTags::Item_Weapon_NoWeapon;
        OnWeaponChanged.Broadcast(nullptr);
    }

    // OnUnequipped: Update UI, stats, etc.
    OnEquipmentChanged.Broadcast(EquipmentType);

    if (EquipSound)
    {
        UGameplayStatics::PlaySound2D(this, EquipSound);
    }

    // Unregister active skills
}

AItem* UEquipmentComponent::GetEquippedItem(FGameplayTag EquipmentType) const
{
    if (const TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(EquipmentType))
    {
        return *EquippedItemPtr;
    }
    return nullptr;
}

UInventoryComponent* UEquipmentComponent::GetInventoryComponent()
{
    if (!CachedInventoryComponent.IsValid())
    {
        if (AActor* OwnerActor = GetOwner())
        {
            CachedInventoryComponent = OwnerActor->FindComponentByClass<UInventoryComponent>();
        }
    }
    return CachedInventoryComponent.Get();
}

const UStateManagerComponent* UEquipmentComponent::GetStateManager()
{
    if (!CachedStateManager.IsValid())
    {
        if (AActor* OwnerActor = GetOwner())
        {
            CachedStateManager = OwnerActor->FindComponentByClass<UStateManagerComponent>();
        }
    }
    return CachedStateManager.Get();
}