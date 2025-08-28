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

    checkf(EquippedItems.Num() == DemoItemTypes::EquipmentTypes.Num(), TEXT("EquippedItems should have all EquipmentTypes."));
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
    APawn* OwnerPawn = GetOwner<APawn>();
    AItem* SpawnedItem = EquipItem_SpawnItem(OwnerPawn, InSlot);
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

    FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UEquipmentComponent::EquipItem"));
    if (!ItemData)
    {
        UE_LOG(LogEquipment, Error, TEXT("EquipItem() - ItemData is not valid."));
        return false;
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

AItem* UEquipmentComponent::EquipItem_SpawnItem(APawn* OwnerPawn, const FItemSlot& InSlot) const
{
    UWorld* World = GetWorld();
    if (!World || !OwnerPawn)
    {
        return nullptr;
    }

    // SpawnDeferred
    // check: What if AItem is inherited by AWeapon, AArmor, etc.?
    FTransform SpawnTransform = FTransform::Identity;
    AItem* SpawnedItem = World->SpawnActorDeferred<AItem>(AItem::StaticClass(), SpawnTransform, OwnerPawn, OwnerPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!SpawnedItem)
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem_SpawnItem() - Failed to spawn deferred."));
        return nullptr;
    }

    // Set properties
    SpawnedItem->SetItemSlot(InSlot);

    // Construction
    UGameplayStatics::FinishSpawningActor(SpawnedItem, SpawnTransform);
    if (UStaticMeshComponent* StaticMesh = SpawnedItem->GetStaticMesh())
    {
        StaticMesh->SetSimulatePhysics(false);
        StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        return SpawnedItem;
    }
    // Destroy without static mesh -> change if there exists no-mesh-ItemType
    else if (IsValid(SpawnedItem))
    {
        UE_LOG(LogEquipment, Warning, TEXT("EquipItem_SpawnItem() - Spawned item has no static mesh."));
        SpawnedItem->Destroy();
        return nullptr;
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
