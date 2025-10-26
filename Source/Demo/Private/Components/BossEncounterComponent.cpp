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

void UBossEncounterComponent::SetupBossPawn(APawn* InBossPawn)
{
    if (BossPawn.IsValid())
    {
        // Unbind previous
        BossPawn->OnDestroyed.RemoveDynamic(this, &ThisClass::HandleBossDestroyed);
    }

    if (IsValid(InBossPawn))
    {
        BossPawn = InBossPawn;
        BossPawn->OnDestroyed.AddDynamic(this, &ThisClass::HandleBossDestroyed);
        CurrentState = EBossEncounterState::Dormant; // @misc - What if player is already in encounter condition?
    }
    else
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("InBossPawn is null."));
        return;
    }
}

void UBossEncounterComponent::StartEncounter(APawn* Instigator)
{
    if (CurrentState != EBossEncounterState::Dormant)
    {
        return;
    }

    if (!Instigator || !BossPawn.IsValid())
    {
        DemoLOG_CF(LogCombat, Warning, TEXT("Instigator or BossPawn is null."));
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
    //OnBossEncounterStarted.Broadcast if needed

    // (UI) Show boss AI status
    DemoPlayerController->ShowBossAIStatus(BossPawn.Get());

    // Activate boss AI
    BossAIController->SetTargetActor(Instigator);

    // @TODO - start music, lock doors, sequence, etc.
}

void UBossEncounterComponent::EndEncounter(EBossEncounterEndReason Reason)
{
    if (CurrentState == EBossEncounterState::Completed)
    {
        return;
    }

    if (Reason == EBossEncounterEndReason::BossDefeated)
    {
        // @misc - If boss is still alive (!BaseCharacter->IsDead()), what to do? Disable further interactions?
        SetEncounterState(EBossEncounterState::Completed);
    }
    else
    {
        SetEncounterState(EBossEncounterState::Dormant);
    }

    //OnBossEncounterEnded.Broadcast if needed

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
    if (BossPawn.IsValid())
    {
        if (ADemoAIController* BossAIController = BossPawn->GetController<ADemoAIController>())
        {
            BossAIController->SetTargetActor(nullptr);
        }
    }

    // @TODO - stop music, unlock doors, sequence, reward, etc.
    // @WARNING - Boss defeated -> Player retreated -> Boss destroyed -> EndEncounter may miss some logics.
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