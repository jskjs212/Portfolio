// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_SetMovementSpeed.generated.h"

USTRUCT()
struct FSTTask_SetMovementSpeedInstanceData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<ACharacter> Character;

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.0"))
    float NewMaxWalkSpeed{500.f};
};

/**
 *
 */
USTRUCT(meta = (DisplayName = "Set Speed", Category = "Demo|Common"))
struct DEMO_API FSTTask_SetMovementSpeed : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_SetMovementSpeedInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    FSTTask_SetMovementSpeed();

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};