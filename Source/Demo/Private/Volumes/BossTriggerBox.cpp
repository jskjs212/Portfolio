// Fill out your copyright notice in the Description page of Project Settings.

#include "Volumes/BossTriggerBox.h"
#include "AIController.h"
#include "Character/PlayerCharacter.h"
#include "Components/BossEncounterComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Pawn.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"

#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif // WITH_EDITORONLY_DATA

ABossTriggerBox::ABossTriggerBox()
{
    // ATriggerBox
    SetHidden(false);
    GetCollisionComponent()->SetHiddenInGame(true);
#if WITH_EDITORONLY_DATA
    if (GetSpriteComponent())
    {
        GetSpriteComponent()->SetHiddenInGame(true);
    }
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
    if (!BossIntroLevelSequence)
    {
        DemoLOG_CF(LogDemoGame, Display, TEXT("No BossIntroLevelSequence assigned for %s."), *GetName());
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

void ABossTriggerBox::StopPlayerMovement(bool bStop)
{
    if (CachedPlayerCharacter.IsValid())
    {
        if (bStop)
        {
            CachedPlayerCharacter->DisableInput(nullptr);
        }
        else
        {
            CachedPlayerCharacter->EnableInput(nullptr);
        }
    }
}

void ABossTriggerBox::SetBossAIPauseState(bool bPause)
{
    if (BossPawn.IsValid())
    {
        if (AAIController* AIController = BossPawn->GetController<AAIController>())
        {
            if (UStateTreeAIComponent* StateTreeAIComponent = AIController->FindComponentByClass<UStateTreeAIComponent>())
            {
                if (bPause)
                {
                    StateTreeAIComponent->PauseLogic(TEXT("BossIntroLevelSequence"));
                }
                else
                {
                    StateTreeAIComponent->ResumeLogic(TEXT("BossIntroLevelSequence"));
                }
            }
        }
    }
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
    CachedPlayerCharacter = Cast<APlayerCharacter>(InInstigator);
    if (!CachedPlayerCharacter.IsValid())
    {
        return;
    }

    // Play the level sequence.
    if (BossIntroLevelSequence)
    {
        ALevelSequenceActor* LevelSequenceActor = nullptr;
        ULevelSequencePlayer* LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, BossIntroLevelSequence, FMovieSceneSequencePlaybackSettings{}, LevelSequenceActor);
        if (LevelSequencePlayer)
        {
            LevelSequencePlayer->Play();

            // Pause character & AI
            bIsPausedForSequence = true;
            StopPlayerMovement(true);
            SetBossAIPauseState(true);

            // Resume AI
            LevelSequencePlayer->OnStop.AddDynamic(this, &ThisClass::HandleBossIntroLevelSequenceStop);
        }
        else
        {
            DemoLOG_CF(LogDemoGame, Warning, TEXT("Failed to play BossIntroLevelSequence for %s."), *GetName());
        }
    }

    // Teleport the instigator to the specified location.
    if (!InstigatorTeleportLocation.IsZero())
    {
        FVector Destination = GetTransform().TransformPosition(InstigatorTeleportLocation);
        const bool bTeleported = CachedPlayerCharacter->TeleportTo(Destination, CachedPlayerCharacter->GetActorRotation());
        if (!bTeleported)
        {
            DemoLOG_CF(LogDemoGame, Display, TEXT("Failed to teleport instigator %s."), *CachedPlayerCharacter->GetName());
        }
    }

    SetEntranceBlocked(true);
}

void ABossTriggerBox::HandleEncounterEnded(EBossEncounterEndReason Reason)
{
    if (bIsPausedForSequence)
    {
        // Resume character & AI
        bIsPausedForSequence = false;
        StopPlayerMovement(false);
        SetBossAIPauseState(false);
    }
    CachedPlayerCharacter = nullptr;
    SetEntranceBlocked(false);
}

void ABossTriggerBox::HandleBossIntroLevelSequenceStop()
{
    if (bIsPausedForSequence)
    {
        // Resume character & AI
        bIsPausedForSequence = false;
        StopPlayerMovement(false);
        SetBossAIPauseState(false);
    }
}