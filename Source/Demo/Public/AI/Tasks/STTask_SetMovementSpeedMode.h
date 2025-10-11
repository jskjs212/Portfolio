// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "STTask_SetMovementSpeedMode.generated.h"

class ABaseCharacter;

USTRUCT()
struct FSTTask_SetMovementSpeedModeInstanceData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<ABaseCharacter> BaseCharacter;

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "Movement.SpeedMode"))
    FGameplayTag NewSpeedMode;
};

/**
 *
 */
USTRUCT(meta = (DisplayName = "Set Speed Mode", Category = "Demo|Common"))
struct DEMO_API FSTTask_SetMovementSpeedMode : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_SetMovementSpeedModeInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    FSTTask_SetMovementSpeedMode();

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};