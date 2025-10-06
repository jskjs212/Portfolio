// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreePropertyRef.h"
#include "STTask_FindNextTargetLocation.generated.h"

class ATargetPoint;

USTRUCT(BlueprintType)
struct FSTTask_FindNextTargetLocationInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Context")
    AActor* Actor;

    // Reference to int32
    UPROPERTY(EditAnywhere, Category = "Input")
    TStateTreePropertyRef<int32> RefToPreviousTargetIndex;

    // Reference to TArray<ATargetPoint*>
    UPROPERTY(EditAnywhere, Category = "Input")
    TStateTreePropertyRef<TArray<ATargetPoint*>> RefToArrayOfTargetPoints;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Output")
    FVector TargetLocation;
};

USTRUCT()
struct DEMO_API FSTTask_FindNextTargetLocation : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_FindNextTargetLocationInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    FSTTask_FindNextTargetLocation();

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};