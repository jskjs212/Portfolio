// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "STTask_PerformAction.generated.h"

USTRUCT()
struct FSTTask_PerformActionInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Context")
    TObjectPtr<APawn> Pawn;

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Categories = "State"))
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bIgnoreCurrentState{false};

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (ClampMin = "0"))
    int32 MontageIndex{0};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bUseRandomIndex{false};
};

/**
 * Succeeds immediately after requesting the action. Fails if it couldn't request the action.
 */
USTRUCT(meta = (DisplayName = "Perform Action", Category = "Demo|Action"))
struct DEMO_API FSTTask_PerformAction : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_PerformActionInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    FSTTask_PerformAction();

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};