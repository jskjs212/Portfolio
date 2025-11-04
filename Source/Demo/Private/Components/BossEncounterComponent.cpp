// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BossEncounterComponent.h"
#include "Audio/DemoAudioSubsystem.h"
#include "AI/DemoAIController.h"
#include "Character/BaseCharacter.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Pawn.h"
#include "PlayerController/DemoPlayerController.h"

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

    // Set state
    SetEncounterState(EBossEncounterState::Active);

    // Member variables
    EncounterInstigator = Instigator;

    // Broadcast
    // e.g. Lock doors, start cutscene, etc.
    OnEncounterStarted.Broadcast(Instigator);

    // Bind to boss death
    if (ABaseCharacter* BossCharacter = Cast<ABaseCharacter>(BossPawn.Get()))
    {
        BossDeathDelegateHandle = BossCharacter->OnDeath.AddUObject(DemoPlayerController, &ADemoPlayerController::HandleBossDeath);
    }

    // (UI) Show boss AI status
    DemoPlayerController->ShowBossAIStatus(BossPawn.Get());

    // Activate boss AI
    DemoAIController->SetTargetActorManually(Instigator);

    // Start music
    if (BossMusicTag.IsValid())
    {
        const UGameInstance* GameInstance = GetOwner() ? GetOwner()->GetGameInstance() : nullptr;
        if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GameInstance))
        {
            AudioSubsystem->PlayMusic(this, BossMusicTag);
        }
    }
}

void UBossEncounterComponent::EndEncounter(EBossEncounterEndReason Reason)
{
    if (CurrentState == EBossEncounterState::Completed)
    {
        return;
    }

    // Set state
    if (Reason == EBossEncounterEndReason::BossDefeated)
    {
        // @misc - If the boss is still alive (!BaseCharacter->IsDead()), what to do? Disable further interactions?
        SetEncounterState(EBossEncounterState::Completed);
    }
    else
    {
        SetEncounterState(EBossEncounterState::Dormant);

        // Regen health immediately (until player hits again)
        if (BossPawn.IsValid())
        {
            if (UStatsComponent* StatsComponent = BossPawn->FindComponentByClass<UStatsComponent>())
            {
                StatsComponent->StartRegenChecked(DemoGameplayTags::Stat_Resource_Health, true);
            }
        }
    }

    // Broadcast
    OnEncounterEnded.Broadcast(Reason);

    // Unbind boss death
    if (ABaseCharacter* BossCharacter = Cast<ABaseCharacter>(BossPawn.Get()))
    {
        BossCharacter->OnDeath.Remove(BossDeathDelegateHandle);
        BossDeathDelegateHandle.Reset();
    }

    // (UI) Hide boss AI status and clear instigator
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
            DemoAIController->SetTargetActorManually(nullptr);
        }
    }

    // Stop music
    const UGameInstance* GameInstance = GetOwner() ? GetOwner()->GetGameInstance() : nullptr;
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GameInstance))
    {
        AudioSubsystem->PlayDefaultMusic(this);
    }
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