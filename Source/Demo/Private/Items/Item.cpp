// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DemoTypes/TableRowBases.h"

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