// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_MoveTo.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"

USTTask_MoveTo::USTTask_MoveTo(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShouldStateChangeOnReselect = false;
    bShouldCallTick = false;
}

EStateTreeRunStatus USTTask_MoveTo::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (Super::EnterState(Context, Transition) == EStateTreeRunStatus::Failed)
    {
        return EStateTreeRunStatus::Failed;
    }

    AAIController* AIController = Pawn ? Pawn->GetController<AAIController>() : nullptr;
    if (!AIController)
    {
        return EStateTreeRunStatus::Failed;
    }

    EPathFollowingRequestResult::Type RequestResult = AIController->MoveToLocation(TargetLocation, AcceptanceRadius);
    switch (RequestResult)
    {
    case EPathFollowingRequestResult::RequestSuccessful:
        AIController->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::HandleMoveCompleted);
        return EStateTreeRunStatus::Running;
    case EPathFollowingRequestResult::AlreadyAtGoal:
        return EStateTreeRunStatus::Succeeded;
    default:
        return EStateTreeRunStatus::Failed;
    }
}

void USTTask_MoveTo::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
    AAIController* AIController = Pawn ? Pawn->GetController<AAIController>() : nullptr;
    if (AIController)
    {
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::HandleMoveCompleted);
    }

    bool bSucceeded = Result == EPathFollowingResult::Success;
    FinishTask(bSucceeded);
}