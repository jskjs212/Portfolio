// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "StateTreeInstanceData.h"
#include "STTask_FindNextTargetLocation.generated.h"

class ATargetPoint;

/**
 *
 */
UCLASS()
class DEMO_API USTTask_FindNextTargetLocation : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Context")
    AActor* Actor;

    UPROPERTY(EditAnywhere, Category = "Input")
    TArray<TObjectPtr<const ATargetPoint>> TargetPoints;

    UPROPERTY(EditAnywhere, Category = "Input")
    int32 CurrentTargetIndex{-1};

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Output")
    FVector TargetLocation;
};