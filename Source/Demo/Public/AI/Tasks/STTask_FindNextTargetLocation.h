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

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<APawn> Pawn;

    // Reference to int32
    UPROPERTY(VisibleAnywhere, Category = "Input")
    TStateTreePropertyRef<int32> RefToPreviousTargetIndex;

    // Reference to TArray<ATargetPoint*>
    UPROPERTY(VisibleAnywhere, Category = "Input")
    TStateTreePropertyRef<TArray<ATargetPoint*>> RefToArrayOfTargetPoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Output")
    FVector TargetLocation{FVector::ZeroVector};
};

USTRUCT(meta = (DisplayName = "Find Next Target Location", Category = "Demo|Common"))
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