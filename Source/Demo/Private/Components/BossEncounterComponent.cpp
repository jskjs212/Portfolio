// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BossEncounterComponent.h"
#include "Audio/DemoAudioSubsystem.h"
#include "AI/DemoAIController.h"
#include "Character/BaseCharacter.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Pawn.h"
#include "PlayerController/DemoPlayerController.h"

UBossEncounterComponent::UBossEncounterComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBossEncounterComponent::SetupBossInfo(APawn* InBossPawn, FGameplayTag InBossMusicTag)
{
    // Unbind previous
    if (BossPawn.IsValid())
    {
        if (ABaseCharacter* PreviousBossCharacter = Cast<ABaseCharacter>(BossPawn.Get()))
        {
            PreviousBossCharacter->OnDeath.Remove(BossDeathDelegateHandle);
            BossDeathDelegateHandle.Reset();
        }
        BossPawn->OnDestroyed.RemoveDynamic(this, &ThisClass::HandleBossDestroyed);
    }

    // Bind new
    // @check - Don't care if the AICharacter::IsBoss() is true or not?
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

    BossMusicTag = InBossMusicTag;
}

void UBossEncounterComponent::StartEncounter(APawn* Instigator)
{
    // Validation start
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

    ADemoAIController* DemoAIController = BossPawn->GetController<ADemoAIController>();
    if (!DemoAIController)
    {
        DemoLOG_CF(LogCombat, Warning, TEXT("BossPawn needs ADemoAIController."));
        return;
    }
    // Validation end

    EncounterInstigator = Instigator;

    SetEncounterState(EBossEncounterState::Active);
    //OnBossEncounterStarted.Broadcast if needed

    // Bind to boss death
    if (ABaseCharacter* BossCharacter = Cast<ABaseCharacter>(BossPawn.Get()))
    {
        BossDeathDelegateHandle = BossCharacter->OnDeath.AddUObject(DemoPlayerController, &ADemoPlayerController::HandleBossDeath);
    }

    // (UI) Show boss AI status
    DemoPlayerController->ShowBossAIStatus(BossPawn.Get());

    // Activate boss AI
    DemoAIController->SetTargetActor(Instigator);

    // Start music
    if (BossMusicTag.IsValid())
    {
        const UGameInstance* GameInstance = GetOwner() ? GetOwner()->GetGameInstance() : nullptr;
        if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GameInstance))
        {
            AudioSubsystem->PlayMusic(this, BossMusicTag);
        }
    }

    // @TODO - Lock doors

    // @TODO - Start sequence
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

    // Unbind boss death
    if (ABaseCharacter* BossCharacter = Cast<ABaseCharacter>(BossPawn.Get()))
    {
        BossCharacter->OnDeath.Remove(BossDeathDelegateHandle);
        BossDeathDelegateHandle.Reset();
    }

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
        if (ADemoAIController* DemoAIController = BossPawn->GetController<ADemoAIController>())
        {
            DemoAIController->SetTargetActor(nullptr);
        }
    }

    // Stop music
    const UGameInstance* GameInstance = GetOwner() ? GetOwner()->GetGameInstance() : nullptr;
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GameInstance))
    {
        AudioSubsystem->PlayDefaultMusic(this);
    }

    // @TODO - unlock doors, sequence, reward (in AICharacter's function?), etc.
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