// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BossEncounterComponent.h"
#include "AI/DemoAIController.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Pawn.h"
#include "PlayerController/DemoPlayerController.h"

UBossEncounterComponent::UBossEncounterComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBossEncounterComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBossEncounterComponent::SetupBossPawn(APawn* InBossPawn)
{
    if (InBossPawn)
    {
        BossPawn = InBossPawn;
        BossPawn->OnDestroyed.AddDynamic(this, &ThisClass::HandleBossDestroyed);
    }
    else
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("InBossPawn is null."));
        return;
    }
}

void UBossEncounterComponent::StartEncounter(APawn* Instigator)
{
    if (!Instigator || !BossPawn)
    {
        DemoLOG_CF(LogCombat, Warning, TEXT("Instigator or BossPawn is null."));
        return;
    }

    if (CurrentState != EBossEncounterState::Idle)
    {
        DemoLOG_CF(LogCombat, Verbose, TEXT("CurrentState is not Idle."));
        return;
    }

    ADemoPlayerController* DemoPlayerController = Instigator->GetController<ADemoPlayerController>();
    if (!DemoPlayerController)
    {
        DemoLOG_CF(LogCombat, Warning, TEXT("Instigator needs ADemoPlayerController."));
        return;
    }

    ADemoAIController* BossAIController = BossPawn->GetController<ADemoAIController>();
    if (!BossAIController)
    {
        DemoLOG_CF(LogCombat, Warning, TEXT("BossPawn needs ADemoAIController."));
        return;
    }

    EncounterInstigator = Instigator;
    SetEncounterState(EBossEncounterState::Active);
    //OnBossEncounterStarted.Broadcast(BossPawn, Instigator);

    // (UI) Show boss AI status
    DemoPlayerController->ShowBossAIStatus(BossPawn);

    // Activate boss AI
    BossAIController->SetTargetActor(Instigator);

    // @TODO - start music, lock doors, sequence, etc.
}

void UBossEncounterComponent::EndEncounter(EBossEncounterEndReason Reason)
{
    if (CurrentState != EBossEncounterState::Active)
    {
        DemoLOG_CF(LogCombat, Verbose, TEXT("Can't end encounter. CurrentState is not Active."));
        return;
    }

    const EBossEncounterState NewState = (Reason == EBossEncounterEndReason::BossDefeated)
        ? EBossEncounterState::Completed : EBossEncounterState::Idle;
    SetEncounterState(NewState);
    //OnBossEncounterEnded.Broadcast(BossPawn, Reason);

    // (UI) Hide boss AI status
    if (EncounterInstigator.IsValid())
    {
        if (ADemoPlayerController* DemoPlayerController = EncounterInstigator->GetController<ADemoPlayerController>())
        {
            DemoPlayerController->ShowBossAIStatus(nullptr);
        }
        EncounterInstigator = nullptr;
    }

    // Deactivate boss AI
    if (ADemoAIController* BossAIController = BossPawn->GetController<ADemoAIController>())
    {
        BossAIController->SetTargetActor(nullptr);
    }

    // @TODO - stop music, unlock doors, sequence, reward, etc.
}

void UBossEncounterComponent::SetEncounterState(EBossEncounterState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        // OnStateChanged if needed
    }
}

void UBossEncounterComponent::HandleBossDestroyed(AActor* DestroyedActor)
{
    EndEncounter(EBossEncounterEndReason::BossDefeated);
}