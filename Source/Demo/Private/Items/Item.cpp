// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DemoTypes/TableRowBases.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
        SpawnedItem->DisableCollision();
    }

    return SpawnedItem;
}

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = false;

    //DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    //SetRootComponent(DefaultSceneRoot);

    // Multiplayer not considered.
    AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
    SetRootComponent(AreaSphere);
    //AreaSphere->SetupAttachment(RootComponent);
    AreaSphere->SetSimulatePhysics(false);
    AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    // TODO: Response only to PickUp trace by the player character.

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetupAttachment(RootComponent);
    StaticMesh->SetVisibility(false);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    SkeletalMesh->SetVisibility(false);
}

void AItem::BeginPlay()
{
    Super::BeginPlay();
}

void AItem::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (ItemSlot.IsValid())
    {
        SetupMesh();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AItem::BeginPlay() - ItemSlot is not set properly from %s."), *GetName());
    }
}

void AItem::SetupMesh()
{
    const FItemDataBase* ItemData = ItemSlot.RowHandle.GetRow<FItemDataBase>(TEXT("AItem::AItem()"));
    if (!ItemData)
    {
        return;
    }

    const auto AttachRules = FAttachmentTransformRules::KeepRelativeTransform;
    const auto DetachRules = FDetachmentTransformRules::KeepWorldTransform;
    UMeshComponent* CurrentMesh = nullptr;

    if (ItemData->SkeletalMesh)
    {
        SkeletalMesh->SetSkeletalMesh(ItemData->SkeletalMesh);
        CurrentMesh = SkeletalMesh;

        if (MeshType != EItemMeshType::SkeletalMesh)
        {
            MeshType = EItemMeshType::SkeletalMesh;

            AreaSphere->DetachFromComponent(DetachRules);
            StaticMesh->DetachFromComponent(DetachRules);
            SkeletalMesh->DetachFromComponent(DetachRules);

            SetRootComponent(SkeletalMesh);
            AreaSphere->AttachToComponent(RootComponent, AttachRules);
            AreaSphere->SetRelativeLocation(FVector::ZeroVector);
            StaticMesh->AttachToComponent(RootComponent, AttachRules);
            StaticMesh->SetRelativeTransform(FTransform::Identity);

            SkeletalMesh->SetSimulatePhysics(true);
            SkeletalMesh->SetCollisionProfileName(TEXT("IgnorePawnAndCamera"));
            SkeletalMesh->SetVisibility(true);

            StaticMesh->SetVisibility(false);
            StaticMesh->SetStaticMesh(nullptr);
        }
    }
    else if (ItemData->StaticMesh)
    {
        StaticMesh->SetStaticMesh(ItemData->StaticMesh);
        CurrentMesh = StaticMesh;

        if (MeshType != EItemMeshType::StaticMesh)
        {
            MeshType = EItemMeshType::StaticMesh;

            AreaSphere->DetachFromComponent(DetachRules);
            StaticMesh->DetachFromComponent(DetachRules);
            SkeletalMesh->DetachFromComponent(DetachRules);

            SetRootComponent(StaticMesh);
            AreaSphere->AttachToComponent(RootComponent, AttachRules);
            AreaSphere->SetRelativeLocation(FVector::ZeroVector);
            SkeletalMesh->AttachToComponent(RootComponent, AttachRules);
            SkeletalMesh->SetRelativeTransform(FTransform::Identity);

            StaticMesh->SetSimulatePhysics(true);
            StaticMesh->SetCollisionProfileName(TEXT("IgnorePawnAndCamera"));
            StaticMesh->SetVisibility(true);

            SkeletalMesh->SetVisibility(false);
            SkeletalMesh->SetSkeletalMesh(nullptr);
        }
    }

    if (CurrentMesh)
    {
        constexpr float AreaSphereScale = 1.1f;
        float Radius;
        FVector TempVector;
        UKismetSystemLibrary::GetComponentBounds(CurrentMesh, TempVector, TempVector, Radius);
        AreaSphere->SetSphereRadius(Radius * AreaSphereScale);
    }
}

void AItem::DisableCollision()
{
    if (MeshType == EItemMeshType::StaticMesh)
    {
        StaticMesh->SetSimulatePhysics(false);
        StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    else if (MeshType == EItemMeshType::SkeletalMesh)
    {
        SkeletalMesh->SetSimulatePhysics(false);
        SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

bool AItem::IsMeshAssetValid() const
{
    if (MeshType == EItemMeshType::StaticMesh)
    {
        return IsValid(StaticMesh->GetStaticMesh());
    }
    return SkeletalMesh && IsValid(SkeletalMesh->GetSkeletalMeshAsset());
}