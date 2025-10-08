// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "STCondition_CanMove.generated.h"

USTRUCT()
struct FSTCondition_CanMoveInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Context")
    TObjectPtr<APawn> Pawn;
};

/**
 * Can move in current state?
 */
USTRUCT(meta = (DisplayName = "Can Move In Current State", Category = "Demo|Action"))
struct DEMO_API FSTCondition_CanMove : public FStateTreeConditionCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTCondition_CanMoveInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
