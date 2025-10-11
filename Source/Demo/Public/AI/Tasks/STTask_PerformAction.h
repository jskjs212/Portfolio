// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "GameplayTagContainer.h"
#include "STTask_PerformAction.generated.h"

/**
 * Succeeds immediately after requesting the action. Fails if it couldn't request the action.
 * Completes when StateManager->OnStateEnded is triggered.
 */
UCLASS(meta = (DisplayName = "Perform Action", Category = "Demo|Action"))
class DEMO_API USTTask_PerformAction : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_PerformAction(const FObjectInitializer& ObjectInitializer);

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

private:
    void HandleStateEnded(FGameplayTag InState);

protected:
    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<APawn> Pawn;

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "State"))
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    bool bIgnoreCurrentState{false};

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0"))
    int32 MontageIndex{0};

    UPROPERTY(EditAnywhere, Category = "Parameter")
    bool bUseRandomIndex{false};

private:
    FDelegateHandle StateEndedHandle;
};