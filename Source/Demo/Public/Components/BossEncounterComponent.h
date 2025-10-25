// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UBossEncounterComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    UBossEncounterComponent();

    void SetupBossPawn(APawn* InBossPawn);

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

    TWeakObjectPtr<APawn> BossPawn;

    TWeakObjectPtr<APawn> EncounterInstigator;
};