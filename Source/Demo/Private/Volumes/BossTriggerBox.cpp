// Fill out your copyright notice in the Description page of Project Settings.

#include "Volumes/BossTriggerBox.h"
#include "Character/PlayerCharacter.h"
#include "Components/BillboardComponent.h"
#include "Components/BossEncounterComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DemoTypes/LogCategories.h"

ABossTriggerBox::ABossTriggerBox()
{
    // ATriggerBox
    SetHidden(false);
    GetCollisionComponent()->SetHiddenInGame(true);
#if WITH_EDITORONLY_DATA
    GetSpriteComponent()->SetHiddenInGame(true);
#endif // WITH_EDITORONLY_DATA

    BossEncounterComponent = CreateDefaultSubobject<UBossEncounterComponent>(TEXT("BossEncounterComponent"));

    EntranceBlockerComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntranceBlockerComponent"));
    EntranceBlockerComponent->SetupAttachment(GetRootComponent());
    SetEntranceBlocked(false);
}

void ABossTriggerBox::BeginPlay()
{
    Super::BeginPlay();

    if (!BossPawn.IsValid())
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("No BossPawn assigned for %s."), *GetName());
        return; // @check - Set boss pawn at runtime?
    }
    if (!BossMusicTag.IsValid())
    {
        DemoLOG_CF(LogDemoGame, Warning, TEXT("No BossMusicTag assigned for %s."), *GetName());
        // Continue
    }
    if (InstigatorTeleportLocation.IsZero())
    {
        DemoLOG_CF(LogDemoGame, Warning, TEXT("No InstigatorTeleportLocation assigned for %s."), *GetName());
        // Continue
    }

    OnActorBeginOverlap.AddDynamic(this, &ThisClass::HandleBeginOverlap);
    OnActorEndOverlap.AddDynamic(this, &ThisClass::HandleEndOverlap);

    BossEncounterComponent->SetupBossInfo(BossPawn.Get(), BossMusicTag);
    BossEncounterComponent->OnEncounterStarted.AddUObject(this, &ThisClass::HandleEncounterStarted);
    BossEncounterComponent->OnEncounterEnded.AddUObject(this, &ThisClass::HandleEncounterEnded);
}

void ABossTriggerBox::SetEntranceBlocked(bool bBlocked)
{
    EntranceBlockerComponent->SetCollisionEnabled(bBlocked ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    EntranceBlockerComponent->SetHiddenInGame(!bBlocked);
}

void ABossTriggerBox::HandleBeginOverlap(AActor* /*OverlappedActor*/, AActor* OtherActor)
{
    // Start the encounter only if the actor is APlayerCharacter.
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
    {
        BossEncounterComponent->StartEncounter(PlayerCharacter);
    }
}

void ABossTriggerBox::HandleEndOverlap(AActor* /*OverlappedActor*/, AActor* OtherActor)
{
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
    {
        BossEncounterComponent->EndEncounter(EBossEncounterEndReason::PlayerRetreated);
    }
}

void ABossTriggerBox::HandleEncounterStarted(APawn* InInstigator)
{
    // Teleport the instigator to the specified location.
    if (!InstigatorTeleportLocation.IsZero())
    {
        FVector Destination = GetTransform().TransformPosition(InstigatorTeleportLocation);
        const bool bTeleported = InInstigator->TeleportTo(Destination, InInstigator->GetActorRotation());
        if (!bTeleported)
        {
            DemoLOG_CF(LogDemoGame, Warning, TEXT("Failed to teleport instigator %s."), *InInstigator->GetName());
        }
    }

    SetEntranceBlocked(true);
}

void ABossTriggerBox::HandleEncounterEnded(EBossEncounterEndReason Reason)
{
    SetEntranceBlocked(false);
}