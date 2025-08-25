// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/EquipmentComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "Items/Item.h"
#include "Kismet/GameplayStatics.h"

UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    EquippedItems.Add(DemoGameplayTags::Item_Weapon, nullptr);
    EquippedItems.Add(DemoGameplayTags::Item_Armor_Shield, nullptr);

    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Weapon, TEXT("MeleeHandSocket"));
    EquipDefaultSocketNames.Add(DemoGameplayTags::Item_Armor_Shield, TEXT("ShieldHandSocket"));
}

void UEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
}

bool UEquipmentComponent::EquipItem(FGameplayTag ItemType, const FItemSlot& ItemSlot)
{
    // TODO: ItemType == ItemSlot's ItemType?
    // Input validation
    if (!ItemSlot.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipItem() - ItemSlot is not valid."));
        return false;
    }

    // Tag validation
    TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(ItemType);
    if (!EquippedItemPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem() - ItemType is not valid."));
        return false;
    }

    // Unequip
    AItem* EquippedItem = *EquippedItemPtr;
    if (EquippedItem)
    {
        if (!UnequipItem(ItemType))
        {
            UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipItem() - Failed to unequip."));
            return false;
        }
    }

    // Spawn
    APawn* OwnerPawn = GetOwner<APawn>();
    AItem* SpawnedItem = SpawnItemToEquip(OwnerPawn, ItemSlot);
    if (!IsValid(SpawnedItem))
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipItem() - Failed to spawn item."));
        return false;
    }

    // TODO: SocketName in FItemData?
    // Socket name
    FName* SocketName = EquipDefaultSocketNames.Find(ItemType);
    if (!SocketName)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem() - No default socket name for %s."), *ItemType.ToString());
        SpawnedItem->Destroy();
        return false;
    }

    // Attach
    if (!AttachActor(SpawnedItem, *SocketName))
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipItem() - Failed to attach item."));
        SpawnedItem->Destroy();
        return false;
    }

    *EquippedItemPtr = SpawnedItem;

    // OnEquipped

    // Update UI, stats, etc.

    // Register active skills

    UE_LOG(LogTemp, Display, TEXT("Equipped - %s."), *ItemSlot.ItemID.RowName.ToString());
    return true;
}

bool UEquipmentComponent::UnequipItem(FGameplayTag ItemType)
{
    // Tag validation
    TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(ItemType);
    if (!EquippedItemPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentComponent::EquipItem() - ItemType is not valid."));
        return false;
    }

    AItem* EquippedItem = *EquippedItemPtr;
    if (!EquippedItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::UnequipItem() - Equipped item not found."));
        return false;
    }

    // TODO: !!!!! Add item to inventory !!!!!

    // Destroy
    if (!EquippedItem->Destroy())
    {
        // TODO: Handle failure, already added to inventory so it should be destroyed
        return false;
    }

    *EquippedItemPtr = nullptr;

    // OnUnequipped

    // Update UI, stats, etc.

    // Unregister active skills

    UE_LOG(LogTemp, Display, TEXT("Unequipped - %s."), *ItemType.ToString());
    return true;
}

AItem* UEquipmentComponent::GetEquippedItem(FGameplayTag ItemType) const
{
    if (const TObjectPtr<AItem>* EquippedItemPtr = EquippedItems.Find(ItemType))
    {
        return *EquippedItemPtr;
    }
    return nullptr;
}

AItem* UEquipmentComponent::SpawnItemToEquip(APawn* OwnerPawn, const FItemSlot& ItemSlot) const
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
            SpawnedItem->SetItemSlot(ItemSlot);
            UGameplayStatics::FinishSpawningActor(SpawnedItem, SpawnTransform);

            // After construction
            if (UStaticMeshComponent* StaticMesh = SpawnedItem->GetStaticMesh())
            {
                StaticMesh->SetSimulatePhysics(false);
                StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

                return SpawnedItem;
            }
            // Destroy without static mesh -> change if there's no-mesh-ItemType
            else if (IsValid(SpawnedItem))
            {
                UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::SpawnItemToEquip() - Spawned item has no static mesh."));
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
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AttachActor() - Socket %s does not exist."), *SocketName.ToString());
        return false;
    }

    const auto AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
    if (!ActorToAttach->AttachToComponent(OwnerMesh, AttachRules, SocketName))
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AttachActor() - Failed to attach %s to %s."), *ActorToAttach->GetName(), *SocketName.ToString());
        return false;
    }

    return true;
}
