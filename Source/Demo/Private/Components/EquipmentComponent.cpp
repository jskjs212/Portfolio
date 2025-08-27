// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/TableRowBases.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "Items/Item.h"
#include "Items/ItemTypes.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogEquipment);

UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    for (const FGameplayTag& EquipmentType : DemoItemTypes::EquipmentTypes)
    {
        EquippedItems.Add(EquipmentType, nullptr);
    }

    // TODO: Config file or data table?
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Weapon, TEXT("MeleeHandSocket"));
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Armor_Shield, TEXT("ShieldHandSocket"));
}

void UEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UEquipmentComponent::EquipItem(const FItemSlot& InSlot)
{
    FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UEquipmentComponent::EquipItem"));
    if (!ItemData)
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - ItemData is not valid."));
        return false;
    }

    FGameplayTag EquipmentType = DemoItemTypes::GetEquipmentType(ItemData->ItemType);
    if (!EquipmentType.IsValid())
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - Item is not equippable."));
        return false;
    }

    // Input validation
    if (!InSlot.IsValid())
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem() - Slot is not valid."));
        return false;
    }

    // Tag validation
    TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(EquipmentType);
    if (!EquippedItemPtr)
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - EquipmentType is not valid."));
        return false;
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
    APawn* OwnerPawn = GetOwner<APawn>();
    AItem* SpawnedItem = SpawnItemToEquip(OwnerPawn, InSlot);
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

    // Register active skills

    UE_LOG(LogEquipment, Display, TEXT("Equipped - %s"), *InSlot.RowHandle.RowName.ToString());
    return true;
}

bool UEquipmentComponent::UnequipItem(FGameplayTag EquipmentType)
{
    // EquipmentType is checked, Unequip requests should come from Equip or UI's existing item.
    AItem* EquippedItem = GetEquippedItem(EquipmentType);
    if (!EquippedItem)
    {
        UE_LOG(LogEquipment, Warning, TEXT("UnequipItem() - Equipped %s not found."), *EquipmentType.ToString());
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

    // Unregister active skills

    UE_LOG(LogEquipment, Display, TEXT("Unequipped - %s"), *EquipmentType.ToString());
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

AItem* UEquipmentComponent::SpawnItemToEquip(APawn* OwnerPawn, const FItemSlot& InSlot) const
{
    if (!OwnerPawn)
    {
        return nullptr;
    }
    if (UWorld* World = GetWorld())
    {
        // check: What if AItem -> AWeapon, AArmor, etc.?
        FTransform SpawnTransform = FTransform::Identity;
        if (AItem* SpawnedItem = World->SpawnActorDeferred<AItem>(AItem::StaticClass(), SpawnTransform, OwnerPawn, OwnerPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
        {
            SpawnedItem->SetItemSlot(InSlot);
            UGameplayStatics::FinishSpawningActor(SpawnedItem, SpawnTransform);

            // After construction
            if (UStaticMeshComponent* StaticMesh = SpawnedItem->GetStaticMesh())
            {
                StaticMesh->SetSimulatePhysics(false);
                StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

                return SpawnedItem;
            }
            // Destroy without static mesh -> change if there exists no-mesh-ItemType
            else if (IsValid(SpawnedItem))
            {
                UE_LOG(LogEquipment, Warning, TEXT("SpawnItemToEquip() - Spawned item has no static mesh."));
                SpawnedItem->Destroy();
                return nullptr;
            }
        }
    }
    return nullptr;
}

bool UEquipmentComponent::AttachActor(AActor* ActorToAttach, FName SocketName) const
{
    ACharacter* OwnerCharacter = GetOwner<ACharacter>();
    if (!ActorToAttach || !OwnerCharacter)
    {
        return false;
    }

    USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
    if (!OwnerMesh->DoesSocketExist(SocketName))
    {
        UE_LOG(LogEquipment, Warning, TEXT("AttachActor() - Socket %s does not exist."), *SocketName.ToString());
        return false;
    }

    const auto AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
    if (!ActorToAttach->AttachToComponent(OwnerMesh, AttachRules, SocketName))
    {
        UE_LOG(LogEquipment, Warning, TEXT("AttachActor() - Failed to attach %s to %s."), *ActorToAttach->GetName(), *SocketName.ToString());
        return false;
    }

    return true;
}
