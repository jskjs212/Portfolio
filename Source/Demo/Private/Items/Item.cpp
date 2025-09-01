// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "Components/MeshComponent.h"
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
    MeshType = EItemMeshType::StaticMesh;

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    SetRootComponent(StaticMesh);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    SkeletalMesh->SetVisibility(false);

    // Multiplayer not considered.
    AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
    AreaSphere->SetupAttachment(RootComponent);
    AreaSphere->SetSimulatePhysics(false);
    AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    // TODO: Response only to PickUp trace by the player character.
}

void AItem::BeginPlay()
{
    Super::BeginPlay();

    if (ItemSlot.IsValid())
    {
        SetupMesh();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AItem::BeginPlay() - ItemSlot is not set properly from %s."), *GetName());
    }
}

#if WITH_EDITOR
void AItem::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    const FName ActiveMemberNodeName = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName();
    if (ActiveMemberNodeName == GET_MEMBER_NAME_CHECKED(AItem, ItemSlot))
    {
        if (ItemSlot.IsValid())
        {
            SetupMesh();
        }
    }
    Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

void AItem::SetupMesh()
{
    check(StaticMesh && SkeletalMesh && AreaSphere);

    const FItemDataBase* ItemData = ItemSlot.RowHandle.GetRow<FItemDataBase>(TEXT("AItem::AItem()"));
    if (!ItemData)
    {
        return;
    }

    const auto AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
    const auto DetachRules = FDetachmentTransformRules::KeepWorldTransform;
    UMeshComponent* CurrentMesh = nullptr;

    if (ItemData->SkeletalMesh)
    {
        if (MeshType != EItemMeshType::SkeletalMesh)
        {
            MeshType = EItemMeshType::SkeletalMesh;

            SkeletalMesh->DetachFromComponent(DetachRules);
            AreaSphere->DetachFromComponent(DetachRules);

            SetRootComponent(SkeletalMesh);

            StaticMesh->AttachToComponent(SkeletalMesh, AttachRules);
            AreaSphere->AttachToComponent(SkeletalMesh, AttachRules);
        }

        SkeletalMesh->SetSkeletalMesh(ItemData->SkeletalMesh);
        SkeletalMesh->SetSimulatePhysics(true);
        SkeletalMesh->SetCollisionProfileName(TEXT("IgnorePawnAndCamera"));
        SkeletalMesh->SetVisibility(true);

        StaticMesh->SetVisibility(false);
        StaticMesh->SetStaticMesh(nullptr);

        CurrentMesh = SkeletalMesh;
    }
    else if (ItemData->StaticMesh)
    {
        if (MeshType != EItemMeshType::StaticMesh)
        {
            MeshType = EItemMeshType::StaticMesh;

            StaticMesh->DetachFromComponent(DetachRules);
            AreaSphere->DetachFromComponent(DetachRules);

            SetRootComponent(StaticMesh);

            SkeletalMesh->AttachToComponent(StaticMesh, AttachRules);
            AreaSphere->AttachToComponent(StaticMesh, AttachRules);
        }

        StaticMesh->SetStaticMesh(ItemData->StaticMesh);
        StaticMesh->SetSimulatePhysics(true);
        StaticMesh->SetCollisionProfileName(TEXT("IgnorePawnAndCamera"));
        StaticMesh->SetVisibility(true);

        SkeletalMesh->SetVisibility(false);
        SkeletalMesh->SetSkeletalMesh(nullptr);

        CurrentMesh = StaticMesh;
    }

    if (CurrentMesh)
    {
        constexpr float AreaSphereScale = 1.1f;
        float Radius;
        FVector TempVector;
        UKismetSystemLibrary::GetComponentBounds(CurrentMesh, TempVector, TempVector, Radius);
        AreaSphere->SetSphereRadius(Radius * AreaSphereScale);
        UE_LOG(LogTemp, Warning, TEXT("AItem::SetupMesh() - Set AreaSphere radius to %f."), Radius);
    }
}

void AItem::DisableCollision()
{
    if (UMeshComponent* CurrentMesh = GetMesh())
    {
        CurrentMesh->SetSimulatePhysics(false);
        CurrentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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