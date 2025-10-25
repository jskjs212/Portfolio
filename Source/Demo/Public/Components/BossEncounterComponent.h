// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossEncounterComponent.generated.h"

UENUM()
enum class EBossEncounterState : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Active UMETA(DisplayName = "Active"),
    Completed UMETA(DisplayName = "Completed")
};

UENUM()
enum class EBossEncounterEndReason : uint8
{
    BossDefeated UMETA(DisplayName = "BossDefeated"),
    PlayerRetreated UMETA(DisplayName = "PlayerRetreated")
};

//DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBossEncounterStarted, APawn* /* BossPawn */, APawn* /* Instigator */);
//DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBossEncounterEnded, APawn* /* BossPawn */, EBossEncounterEndReason /* Reason */);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UBossEncounterComponent : public UActorComponent
{
    GENERATED_BODY()

//public:
//    FOnBossEncounterStarted OnBossEncounterStarted;
//    FOnBossEncounterEnded OnBossEncounterEnded;

public:
    UBossEncounterComponent();

protected:
    virtual void BeginPlay() override;

public:
    void SetupBossPawn(APawn* InBossPawn);

    void StartEncounter(APawn* Instigator);

    void EndEncounter(EBossEncounterEndReason Reason);

private:
    void SetEncounterState(EBossEncounterState NewState);

    UFUNCTION()
    void HandleBossDestroyed(AActor* DestroyedActor);

private:
    UPROPERTY(VisibleAnywhere, Category = "Boss Encounter")
    EBossEncounterState CurrentState{EBossEncounterState::Idle};

    TObjectPtr<APawn> BossPawn;

    TWeakObjectPtr<APawn> EncounterInstigator;
};