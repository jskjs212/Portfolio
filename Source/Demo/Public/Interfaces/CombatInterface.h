// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DemoTypes/AttackTypes.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class DEMO_API ICombatInterface
{
    GENERATED_BODY()

public:
    /* Functions */

    // @param bIgnoreState: If true, ignore current state.
    // @return Duration of the action's AnimMontage, or 0.f if failed to perform action.
    virtual float PerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex = false) = 0;

    /* Get & set */

    virtual bool IsInAction(FGameplayTag InAction) const = 0;

    virtual bool CanReceiveDamageFrom(const AActor* Attacker) const = 0;

    virtual float CalculateDamage(EAttackCollisionType InType) const = 0;

    virtual int32 GetActionInfoCount(FGameplayTag InAction) const = 0;

    // @return Desired control rotation while performing action.
    virtual FRotator GetDesiredControlRotation() const = 0;

    // @return Desired input rotation while performing action.
    virtual FRotator GetDesiredInputRotation() const = 0;
};