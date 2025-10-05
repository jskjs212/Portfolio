// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "GameplayTagContainer.h"
#include "STTask_PerformAction.generated.h"

/**
 *
 */
UCLASS()
class DEMO_API USTTask_PerformAction : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Context")
    AActor* Actor;

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Categories = "State"))
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bIgnoreCurrentState{false};

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (ClampMin = "0"))
    int32 MontageIndex{0};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bUseRandomIndex{false};
};