// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "Components/StaticMeshComponent.h"
#include "DemoTypes/TableRowBases.h"
#include "Kismet/GameplayStatics.h"

AItem* AItem::SpawnItem(
    UWorld* World,
    const FItemSlot& InSlot,
    const FTransform& SpawnTransform,
    bool bDisableCollision,
    AActor* Owner,
    APawn* Instigator,
    ESpawnActorCollisionHandlingMethod CollisionHandlingMethod
)
{
    if (!World || !InSlot.IsValid())
    {
        return nullptr;
    }

    // SpawnDeferred
    AItem* SpawnedItem = World->SpawnActorDeferred<AItem>(AItem::StaticClass(), SpawnTransform, Owner, Instigator, CollisionHandlingMethod);
    if (!SpawnedItem)
    {
        return nullptr;
    }

    // Set properties
    SpawnedItem->SetItemSlot(InSlot);

    // Construction
    UGameplayStatics::FinishSpawningActor(SpawnedItem, SpawnTransform);

    if (bDisableCollision)
    {
        if (SpawnedItem->StaticMesh->GetStaticMesh())
        {
            SpawnedItem->StaticMesh->SetSimulatePhysics(false);
            SpawnedItem->StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
    return SpawnedItem;
}

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = false;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    SetRootComponent(StaticMesh);
}

void AItem::BeginPlay()
{
    Super::BeginPlay();

    if (!ItemSlot.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("AItem::BeginPlay() - ItemSlot is not set properly from %s."), *GetName());
    }
}

void AItem::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Set static mesh from ItemSlot
    const FItemDataBase* ItemData = ItemSlot.RowHandle.GetRow<FItemDataBase>(TEXT("AItem::AItem()"));
    if (ItemData && ItemData->StaticMesh)
    {
        StaticMesh->SetStaticMesh(ItemData->StaticMesh);
        StaticMesh->SetSimulatePhysics(true);
        StaticMesh->SetCollisionProfileName(TEXT("IgnorePawnAndCamera"));
        StaticMesh->SetVisibility(true);
    }
    else
    {
        StaticMesh->SetStaticMesh(nullptr);
        StaticMesh->SetVisibility(false);
    }
}

bool AItem::IsStaticMeshValid() const
{
    return StaticMesh && IsValid(StaticMesh->GetStaticMesh());
}