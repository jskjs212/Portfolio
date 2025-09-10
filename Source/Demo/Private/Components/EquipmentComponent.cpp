// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Character.h"
#include "Items/Item.h"
#include "Items/ItemTypes.h"
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

    // TODO: Config file or data table?
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Weapon, TEXT("MeleeHandSocket"));
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Armor_Shield, TEXT("ShieldHandSocket"));

    checkf(EquippedItems.Num() == DemoItemTypes::GetEquipmentTypes().Num(), TEXT("EquippedItems should have all EquipmentTypes."));
}

bool UEquipmentComponent::EquipItem(const FItemSlot& InSlot)
{
    FGameplayTag EquipmentType;
    TObjectPtr<AItem>* EquippedItemPtr;

    // Validate and get data
    bool bValid = EquipItem_Validate(InSlot, EquipmentType, EquippedItemPtr);
    if (!bValid)
    {
        return false; // Log in EquipItem_Validate()
    }

    // Unequip
    AItem* EquippedItem = *EquippedItemPtr;
    if (EquippedItem)
    {
        if (!UnequipItem(EquipmentType))
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

    // TODO: SocketName in FItemData?
    // Socket name
    FName* SocketName = EquipDefaultSocketNames.Find(EquipmentType);
    if (!SocketName)
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - No default socket name for %s."), *EquipmentType.ToString());
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

    *EquippedItemPtr = SpawnedItem;

    // OnEquipped

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
        if (InventoryComp->AddItem(EquippedItem->GetItemSlot()) <= 0)
        {
            // misc: In-game notification?
            UE_LOG(LogEquipment, Warning, TEXT("UnequipItem() - Failed to add item to inventory."));
            return false;
        }
    }

    // Destroy
    if (!EquippedItem->Destroy())
    {
        // TODO: Handle failure, already added to inventory so it should be destroyed
        UE_LOG(LogEquipment, Error, TEXT("UnequipItem() - Failed to destroy item."));
        return false;
    }

    EquippedItems[EquipmentType] = nullptr;

    // OnUnequipped

    // Update UI, stats, etc.
    if (EquipSound)
    {
        UGameplayStatics::PlaySound2D(this, EquipSound);
    }

    // Unregister active skills

    UE_LOG(LogEquipment, Display, TEXT("Unequipped - %s"), *EquipmentType.ToString());
    return true;
}

bool UEquipmentComponent::EquipItem_Validate(const FItemSlot& InSlot, FGameplayTag& OutEquipmentType, TObjectPtr<AItem>*& OutEquippedItemPtr)
{
    if (!InSlot.IsValid())
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Slot is not valid."));
        return false;
    }

    FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UEquipmentComponent::EquipItem_Validate"));
    if (!ItemData)
    {
        return false; // Log in GetRow()
    }

    OutEquipmentType = DemoItemTypes::GetEquipmentType(ItemData->ItemType);
    if (!OutEquipmentType.IsValid())
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - Item is not equippable."));
        return false;
    }

    // Tag validation
    OutEquippedItemPtr = EquippedItems.Find(OutEquipmentType);
    if (!OutEquippedItemPtr)
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - EquipmentType is not valid."));
        return false;
    }

    return true;
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