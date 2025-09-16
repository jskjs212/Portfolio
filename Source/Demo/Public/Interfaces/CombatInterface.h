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
    virtual void UpdateActionInfo(FGameplayTag WeaponTag) PURE_VIRTUAL(ICombatInterface::UpdateActionInfo);
};