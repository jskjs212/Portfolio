// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "CombatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Combat functions
    ////////////////////////////////////////////////////////
public:
    UFUNCTION(BlueprintCallable)
    float ToggleCombat();

    UFUNCTION(BlueprintCallable)
    float Attack(FGameplayTag AttackAction);

    // Only called by animation notify. Force to perform action.
    UFUNCTION(BlueprintCallable)
    float ContinueAttack(FGameplayTag AttackAction);

private:
    void ResetCombat();

    void HandleStateEnded(FGameplayTag InState);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE bool IsCombatReady() const { return !bCanToggleCombat || bIsCombatReady; }

    FORCEINLINE int32 GetCurrentActionIndex() const { return CurrentActionIndex; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization|Combat")
    bool bCanToggleCombat{false};

    // Is weapon hand equipped? (Not sheathed)
    bool bIsCombatReady{false};

    // Save attack input for combo system.
    bool bIsAttackBuffered{false};

    // Current index of any action being performed.
    int32 CurrentActionIndex{0};

    // Index of next attack.
    int32 AttackIndex{0};
};