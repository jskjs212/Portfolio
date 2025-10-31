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

/**
 * Handles boss encounter state and logic.
 * This component can be added to an actor that manages boss encounters.
 * e.g. BossTriggerBox, BossTriggerSwitch, Door, etc.
 *
 * @TODO - Boss defeated -> Player retreated -> Boss destroyed -> EndEncounter may miss some logics.
 * @TODO - Apply DOT effects to boss -> Player retreated -> Boss defeated -> EndEncounter may miss some logics.
 * @WARNING - Player should not be allowed to kill boss outside of encounter.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UBossEncounterComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    UBossEncounterComponent();

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
    UPROPERTY(VisibleAnywhere, Category = "Boss Encounter")
    EBossEncounterState CurrentState{EBossEncounterState::Dormant};

    FGameplayTag BossMusicTag;

    FDelegateHandle BossDeathDelegateHandle;

    TWeakObjectPtr<APawn> BossPawn;

    TWeakObjectPtr<APawn> EncounterInstigator;
};