// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STTask_MoveTo.generated.h"

namespace EPathFollowingResult { enum Type : int; }
class AAIController;

/**
 * Simple version of MoveTo task (inflexible) using AAIController::MoveTo.
 * Completes when AAIController::ReceiveMoveCompleted is triggered.
 * Priority: TargetActor > TargetLocation.
 */
UCLASS(meta = (DisplayName = "Simple Move To", Category = "AI|Action"))
class DEMO_API USTTask_MoveTo : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()

public:
    USTTask_MoveTo(const FObjectInitializer& ObjectInitializer);

protected:
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;

private:
    // Bound to AAIController::ReceiveMoveCompleted delegate, which is called when MoveTo request is finished.
    UFUNCTION()
    void HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

protected:
    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<APawn> Pawn;

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TWeakObjectPtr<AAIController> AIController;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    TObjectPtr<AActor> TargetActor;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    FVector TargetLocation;

    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0."))
    float AcceptanceRadius{10.f};
};