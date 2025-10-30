// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_FindRandomLocation.generated.h"

USTRUCT(BlueprintType)
struct FSTTask_FindRandomLocationInstanceData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<APawn> Pawn;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    float SearchRadius{1000.f};

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Output")
    FVector RandomLocation{FVector::ZeroVector};
};

/**
 *
 */
USTRUCT(meta = (DisplayName = "Find Random Location", Category = "Demo|Common"))
struct DEMO_API FSTTask_FindRandomLocation : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_FindRandomLocationInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    FSTTask_FindRandomLocation();

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};