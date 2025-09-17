// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
    // @return Duration of the action's AnimMontage, or 0.f if failed to perform action.
    virtual float PerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex = false) PURE_VIRTUAL(ICombatInterface::PerformAction, return 0.f;);

    virtual bool IsInAction(FGameplayTag InAction) const PURE_VIRTUAL(ICombatInterface::IsInAction, return false;);

    virtual int32 GetActionInfoCount(FGameplayTag InAction) const PURE_VIRTUAL(ICombatInterface::GetActionInfoCount, return 0;);
};