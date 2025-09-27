// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/ItemTypes.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Character.h"
#include "Items/Item.h"
#include "PlayerController/DemoPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ItemActionDispatcher.h"

DEFINE_LOG_CATEGORY(LogEquipment);

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
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Failed to spawn item."));
        return false;
    }

    // Attach
    if (!AttachActor(SpawnedItem, ValidationResult.EquipmentType))
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Failed to attach item."));
        SpawnedItem->Destroy();
        return false;
    }

    *(ValidationResult.EquippedItemPtr) = SpawnedItem;

    EquipItem_PostProcess(ValidationResult);

    UE_LOG(LogEquipment, Display, TEXT("Equipped - %s"), *InSlot.RowHandle.RowName.ToString());
    return true;
}

bool UEquipmentComponent::UnequipItem(const FGameplayTag EquipmentType)
{
    AItem* EquippedItem = GetEquippedItem(EquipmentType);
    if (!EquippedItem)
    {
        UE_LOG(LogEquipment, Display, TEXT("UnequipItem() - Not equipped %s."), *EquipmentType.ToString());
        return false;
    }

    // Add to inventory if possible
    UInventoryComponent* InventoryComp = GetOwner()->FindComponentByClass<UInventoryComponent>();
    if (InventoryComp)
    {
        FItemActionRequest Request;
        Request.Slot = EquippedItem->GetItemSlot();
        Request.Quantity = 1;

        if (InventoryComp->AddItem(Request) <= 0)
        {
            // @misc - In-game notification?
            UE_LOG(LogEquipment, Warning, TEXT("UnequipItem() - Failed to add item to inventory."));
            return false;
        }
    }

    // Destroy
    if (!EquippedItem->Destroy())
    {
        // @TODO - Handle failure, already added to inventory so it should be destroyed
        UE_LOG(LogEquipment, Error, TEXT("UnequipItem() - Failed to destroy item."));
        return false;
    }

    EquippedItems[EquipmentType] = nullptr;

    UnequipItem_PostProcess(EquipmentType);

    UE_LOG(LogEquipment, Display, TEXT("Unequipped - %s"), *EquipmentType.ToString());
    return true;
}

bool UEquipmentComponent::UnequipAndDropItem(FGameplayTag EquipmentType)
{
    AItem* EquippedItem = GetEquippedItem(EquipmentType);
    if (!EquippedItem)
    {
        UE_LOG(LogEquipment, Display, TEXT("UnequipAndDropItem() - Not equipped %s."), *EquipmentType.ToString());
        return false;
    }

    // Detach
    EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // Drop
    bool bDropped = EquippedItem->Drop(GetOwner());
    if (!bDropped) // Unlikely, but just in case
    {
        UE_LOG(LogEquipment, Error, TEXT("UnequipAndDropItem() - Failed to drop item."));
        AttachActor(EquippedItem, EquipmentType);
        return false;
    }

    EquippedItems[EquipmentType] = nullptr;

    UnequipItem_PostProcess(EquipmentType);

    UE_LOG(LogEquipment, Display, TEXT("Unequipped - %s"), *EquipmentType.ToString());
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
    if (!InSlot.IsValid())
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Slot is not valid."));
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
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - Item is not equippable."));
        return {};
    }

    // Tag validation
    TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(EquipmentType);
    if (!EquippedItemPtr)
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - EquipmentType is not valid."));
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
            UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Failed to unequip."));
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
                UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Failed to unequip shield."));
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
            UE_LOG(LogEquipment, Display, TEXT("EquipItem() - Can't equip shield when two-handing."));
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
        UE_LOG(LogEquipment, Error, TEXT("EquipItem_SpawnItem() - Spawned item has no mesh."));
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
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - No default socket name for %s."), *EquipmentType.ToString());
        return false;
    }
    const FName SocketName = *SocketNamePtr;

#if WITH_EDITOR
    if (!OwnerMesh->DoesSocketExist(SocketName))
    {
        UE_LOG(LogEquipment, Warning, TEXT("AttachActor() - Socket %s does not exist."), *SocketName.ToString());
        return false;
    }
#endif // WITH_EDITOR

    const auto AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
    if (!ActorToAttach->AttachToComponent(OwnerMesh, AttachRules, SocketName))
    {
        UE_LOG(LogEquipment, Warning, TEXT("AttachActor() - Failed to attach %s to %s."), *ActorToAttach->GetName(), *SocketName.ToString());
        return false;
    }

    return true;
}

void UEquipmentComponent::EquipItem_PostProcess(const FEquipmentValidationResult& ValidationResult)
{
    CurrentWeaponType = ValidationResult.ItemType;

    // OnEquipped: Update UI, stats, etc.
    OnEquipmentChanged.Broadcast(ValidationResult.EquipmentType);
    if (ValidationResult.EquipmentType == DemoGameplayTags::Item_Weapon)
    {
        OnWeaponChanged.Broadcast(ValidationResult.ItemType);
    }

    if (EquipSound)
    {
        UGameplayStatics::PlaySound2D(this, EquipSound);
    }

    // Register active skills
}

void UEquipmentComponent::UnequipItem_PostProcess(FGameplayTag EquipmentType)
{
    CurrentWeaponType = DemoGameplayTags::Item_Weapon_NoWeapon;

    // OnUnequipped: Update UI, stats, etc.
    OnEquipmentChanged.Broadcast(EquipmentType);
    if (EquipmentType == DemoGameplayTags::Item_Weapon)
    {
        OnWeaponChanged.Broadcast(DemoGameplayTags::Item_Weapon_NoWeapon);
    }

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