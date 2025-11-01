// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "BossEncounterComponent.generated.h"

UENUM()
enum class EBossEncounterState : uint8
{
    Dormant UMETA(DisplayName = "Dormant"),
    Active UMETA(DisplayName = "Active"),
    Completed UMETA(DisplayName = "Completed")
};

UENUM()
enum class EBossEncounterEndReason : uint8
{
    BossDefeated UMETA(DisplayName = "BossDefeated"),
    PlayerRetreated UMETA(DisplayName = "PlayerRetreated")
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEncounterStarted, APawn* /* Instigator */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEncounterEnded, EBossEncounterEndReason /* Reason */);

/**
 * Handles common logics for boss encounters; state management, activate AI, show UI, play music, etc.
 * This component can be added to an actor that triggers boss encounters.
 * e.g. BossTriggerBox, BossTriggerSwitch, Door, etc.
 *
 * @TODO - Boss defeated -> Player retreated -> Boss destroyed -> EndEncounter may miss some logics.
 * @TODO - Apply DOT effects to boss -> Player retreated -> Boss defeated -> EndEncounter may miss some logics.
 * @WARNING - Boss kill outside of this encounter causes undefined behavior.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UBossEncounterComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnEncounterStarted OnEncounterStarted;
    FOnEncounterEnded OnEncounterEnded;

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    // Sets up the boss pawn and music tag for the encounter.
    // @param InBossMusicTag Don't play music if invalid.
    // Music is for all bosses regardless of the owner of this component, so it is set here.
    void SetupBossInfo(APawn* InBossPawn, FGameplayTag InBossMusicTag);

    void StartEncounter(APawn* Instigator);

    void EndEncounter(EBossEncounterEndReason Reason);

private:
    void SetEncounterState(EBossEncounterState NewState);

    UFUNCTION()
    void HandleBossDestroyed(AActor* DestroyedActor);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    EBossEncounterState CurrentState{EBossEncounterState::Dormant};

    FGameplayTag BossMusicTag;

    FDelegateHandle BossDeathDelegateHandle;

    TWeakObjectPtr<APawn> BossPawn;

    TWeakObjectPtr<APawn> EncounterInstigator;
};