// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_MoveTo.generated.h"

namespace EPathFollowingResult { enum Type : int; }

/**
 * HandleMoveCompleted() is bound to AAIController::ReceiveMoveCompleted delegate, which is called when MoveTo request is finished.
 */
UCLASS()
class DEMO_API USTTask_MoveTo : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_MoveTo(const FObjectInitializer& ObjectInitializer);

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

private:
    UFUNCTION()
    void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

protected:
    UPROPERTY(EditAnywhere, Category = "Context")
    APawn* Pawn;

    UPROPERTY(EditAnywhere, Category = "Input")
    FVector TargetLocation;

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (ClampMin = "0."))
    float AcceptanceRadius{10.0f};
};