// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/ItemTypes.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Character.h"
#include "Items/Item.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogEquipment);

UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();

    for (const FGameplayTag& EquipmentType : DemoItemTypes::GetEquipmentTypes())
    {
        EquippedItems.Add(EquipmentType, nullptr);
    }

    // @TODO - Config file or data table?
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Weapon, TEXT("MeleeHandSocket"));
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Armor_Shield, TEXT("ShieldHandSocket"));

    checkf(EquippedItems.Num() == DemoItemTypes::GetEquipmentTypes().Num(), TEXT("EquippedItems should have all EquipmentTypes."));
}

bool UEquipmentComponent::EquipItem(const FItemSlot& InSlot)
{
    // Validate and get data
    FEquipmentValidationData ValidationResult = EquipItem_Validate(InSlot);
    if (!ValidationResult.bIsValid)
    {
        return false; // Log in EquipItem_Validate()
    }

    // Unequip
    AItem* EquippedItem = *(ValidationResult.EquippedItemPtr);
    if (EquippedItem)
    {
        if (!UnequipItem(ValidationResult.EquipmentType))
        {
            UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Failed to unequip."));
            return false;
        }
    }

    // Spawn
    AItem* SpawnedItem = EquipItem_SpawnItem(InSlot);
    if (!IsValid(SpawnedItem))
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Failed to spawn item."));
        return false;
    }

    // @TODO - SocketName in FItemData?
    // Socket name
    FName* SocketName = EquipDefaultSocketNames.Find(ValidationResult.EquipmentType);
    if (!SocketName)
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - No default socket name for %s."), *ValidationResult.EquipmentType.ToString());
        SpawnedItem->Destroy();
        return false;
    }

    // Attach
    if (!AttachActor(SpawnedItem, *SocketName))
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Failed to attach item."));
        SpawnedItem->Destroy();
        return false;
    }

    *(ValidationResult.EquippedItemPtr) = SpawnedItem;

    // OnEquipped
    if (ValidationResult.EquipmentType == DemoGameplayTags::Item_Weapon)
    {
        OnWeaponChanged.ExecuteIfBound(ValidationResult.ItemType);
    }

    // Update UI, stats, etc.
    if (EquipSound)
    {
        UGameplayStatics::PlaySound2D(this, EquipSound);
    }

    // Register active skills

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

    // OnUnequipped
    if (EquipmentType == DemoGameplayTags::Item_Weapon)
    {
        OnWeaponChanged.ExecuteIfBound(DemoGameplayTags::Item_Weapon_NoWeapon);
    }

    // Update UI, stats, etc.
    if (EquipSound)
    {
        UGameplayStatics::PlaySound2D(this, EquipSound);
    }

    // Unregister active skills

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

FEquipmentValidationData UEquipmentComponent::EquipItem_Validate(const FItemSlot& InSlot)
{
    if (!InSlot.IsValid())
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Slot is not valid."));
        return {};
    }

    FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UEquipmentComponent::EquipItem_Validate"));
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

    FEquipmentValidationData Result;
    Result.bIsValid = true;
    Result.ItemType = ItemType;
    Result.EquipmentType = EquipmentType;
    Result.EquippedItemPtr = EquippedItemPtr;

    return Result;
}

AItem* UEquipmentComponent::GetEquippedItem(FGameplayTag EquipmentType) const
{
    if (const TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(EquipmentType))
    {
        return *EquippedItemPtr;
    }
    return nullptr;
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

bool UEquipmentComponent::AttachActor(AActor* ActorToAttach, const FName SocketName) const
{
    ACharacter* OwnerCharacter = GetOwner<ACharacter>();
    if (!ActorToAttach || !OwnerCharacter)
    {
        return false;
    }

    USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();

#if WITH_EDITOR // Debug
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