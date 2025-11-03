// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Components/BoxComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DemoTypes/LogCategories.h"
#include "Items/ItemRowBases.h"
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

int32 AItem::DropItem(UWorld* World, const FItemSlot& InSlot, const AActor* Dropper)
{
    if (!World || !Dropper)
    {
        return -1;
    }

    // Spawn location
    const FVector LocationOffset = Dropper->GetActorForwardVector() * DropDistance + FVector{0.f, 0.f, DropHeight};
    FTransform SpawnTransform = Dropper->GetActorTransform();
    SpawnTransform.AddToTranslation(LocationOffset);

    // Spawn
    AItem* DroppedItem = AItem::SpawnItem(World, InSlot, SpawnTransform);
    if (!IsValid(DroppedItem))
    {
        DemoLOG_CF(LogTemp, Error, TEXT("Failed to spawn item."));
        return -1;
    }

    // Check mesh to drop
    if (!DroppedItem->IsMeshAssetValid())
    {
        DemoLOG_CF(LogTemp, Error, TEXT("Item has no mesh."));
        DroppedItem->Destroy();
        return -1;
    }

    // Throw
    DroppedItem->GetMesh()->AddImpulse(Dropper->GetActorForwardVector() * DropImpulseStrength, NAME_None, true);

    // Play sound
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(DroppedItem->GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(DroppedItem, DemoSoundTags::SFX_Item_Drop);
    }

    return InSlot.Quantity;
}

AItem::AItem()
{
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    SetRootComponent(StaticMesh);
    StaticMesh->SetVisibility(false);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    SkeletalMesh->SetVisibility(false);

    // Multiplayer not considered.
    InteractCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractCollision"));
    InteractCollision->SetupAttachment(RootComponent);
    InteractCollision->SetSimulatePhysics(false);
    InteractCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // Block visibility to prevent line trace through objects.
    InteractCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
        DemoLOG_CF(LogTemp, Error, TEXT("ItemSlot is not set properly from %s."), *GetName());
    }
}

#if WITH_EDITOR
void AItem::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    // Update mesh when ItemSlot is changed in editor.
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

bool AItem::Drop(const AActor* Dropper)
{
    UWorld* World = GetWorld();
    if (!World || !Dropper || !IsMeshAssetValid())
    {
        return false;
    }

    SimulatePhysics();

    // Spawn location
    const FVector LocationOffset = Dropper->GetActorForwardVector() * DropDistance + FVector{0.f, 0.f, DropHeight};
    FTransform NewTransform = Dropper->GetActorTransform();
    NewTransform.AddToTranslation(LocationOffset);
    SetActorTransform(NewTransform);

    // Throw
    GetMesh()->AddImpulse(Dropper->GetActorForwardVector() * DropImpulseStrength, NAME_None, true);

    return true;
}

void AItem::SimulatePhysics()
{
    UMeshComponent* CurrentMesh = GetMesh();
    if (CurrentMesh)
    {
        CurrentMesh->SetSimulatePhysics(true);
        CurrentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
}

void AItem::SetupMesh()
{
    check(StaticMesh && SkeletalMesh && InteractCollision);

    const FItemDataBase* ItemData = ItemSlot.RowHandle.GetRow<FItemDataBase>(TEXT("AItem::AItem()"));
    if (!ItemData)
    {
        return;
    }

    const auto AttachRules = FAttachmentTransformRules::KeepRelativeTransform;
    const auto DetachRules = FDetachmentTransformRules::KeepRelativeTransform;
    UMeshComponent* CurrentMesh = nullptr;

    // Set skeletal mesh.
    if (ItemData->SkeletalMesh)
    {
        if (MeshType != EItemMeshType::SkeletalMesh)
        {
            MeshType = EItemMeshType::SkeletalMesh;

            const FTransform& CurrentTransform = GetActorTransform();

            SkeletalMesh->DetachFromComponent(DetachRules);
            InteractCollision->DetachFromComponent(DetachRules);

            SetRootComponent(SkeletalMesh);
            SetActorTransform(CurrentTransform, false, nullptr, ETeleportType::ResetPhysics);

            StaticMesh->AttachToComponent(SkeletalMesh, AttachRules);
            StaticMesh->SetRelativeTransform(FTransform::Identity);

            InteractCollision->AttachToComponent(SkeletalMesh, AttachRules);
            InteractCollision->SetRelativeTransform(FTransform::Identity);
        }

        SkeletalMesh->SetSkeletalMesh(ItemData->SkeletalMesh);
        SkeletalMesh->SetSimulatePhysics(true);
        SkeletalMesh->SetCollisionProfileName(TEXT("IgnorePawnAndCamera"));
        SkeletalMesh->SetVisibility(true);

        StaticMesh->SetVisibility(false);
        StaticMesh->SetStaticMesh(nullptr);

        CurrentMesh = SkeletalMesh;
    }
    // Set static mesh.
    else if (ItemData->StaticMesh)
    {
        if (MeshType != EItemMeshType::StaticMesh)
        {
            MeshType = EItemMeshType::StaticMesh;

            const FTransform& CurrentTransform = GetActorTransform();

            StaticMesh->DetachFromComponent(DetachRules);
            InteractCollision->DetachFromComponent(DetachRules);

            SetRootComponent(StaticMesh);
            SetActorTransform(CurrentTransform);

            SkeletalMesh->AttachToComponent(StaticMesh, AttachRules);
            SkeletalMesh->SetRelativeTransform(FTransform::Identity, false, nullptr, ETeleportType::ResetPhysics);

            InteractCollision->AttachToComponent(StaticMesh, AttachRules);
            InteractCollision->SetRelativeTransform(FTransform::Identity);
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
        float Radius;
        FVector Origin;
        FVector BoxExtent;
        UKismetSystemLibrary::GetComponentBounds(CurrentMesh, Origin, BoxExtent, Radius);
        InteractCollision->SetBoxExtent(BoxExtent);
        // @TEST
        //DemoLOG_CF(LogTemp, Display, TEXT("BoxExtent: %s, Origin: %s."), *BoxExtent.ToString(), *Origin.ToString());
    }
}

void AItem::DisableCollision()
{
    UMeshComponent* CurrentMesh = GetMesh();
    if (CurrentMesh && InteractCollision)
    {
        CurrentMesh->SetSimulatePhysics(false);
        CurrentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        InteractCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AItem::Interact(APawn* InstigatorPawn)
{
    if (InstigatorPawn)
    {
        if (UInventoryComponent* InventoryComp = InstigatorPawn->FindComponentByClass<UInventoryComponent>())
        {
            // Index = -1
            FItemActionRequest Request;
            Request.Slot = ItemSlot;
            Request.Quantity = ItemSlot.Quantity;

            const int32 Added = InventoryComp->AddItem(Request);
            ItemSlot.Quantity -= Added;

            if (Added > 0)
            {
                if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
                {
                    AudioSubsystem->PlaySound2D(this, DemoSoundTags::SFX_Item_PickUp);
                }
            }

            if (ItemSlot.Quantity <= 0)
            {
                Destroy();
            }
        }
    }
}

void AItem::ShowHighlight(bool bShow)
{
    UMeshComponent* CurrentMesh = GetMesh();
    if (CurrentMesh)
    {
        CurrentMesh->SetRenderCustomDepth(bShow);
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