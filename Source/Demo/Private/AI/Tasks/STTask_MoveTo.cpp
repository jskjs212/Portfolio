// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_MoveTo.h"
#include "AIController.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"

USTTask_MoveTo::USTTask_MoveTo(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShouldCallTick = false;
}

EStateTreeRunStatus USTTask_MoveTo::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (Super::EnterState(Context, Transition) == EStateTreeRunStatus::Failed)
    {
        return EStateTreeRunStatus::Failed;
    }

    if (!AIController)
    {
        DemoLOG_CF(LogAI, Error, TEXT("Invalid AIController for %s"), *GetNameSafe(Pawn));
        return EStateTreeRunStatus::Failed;
    }

    EPathFollowingRequestResult::Type RequestResult;
    if (TargetActor)
    {
        RequestResult = AIController->MoveToActor(TargetActor, AcceptanceRadius);
    }
    else if (!TargetLocation.IsZero())
    {
        RequestResult = AIController->MoveToLocation(TargetLocation, AcceptanceRadius);
    }
    else
    {
        // @TEST - This can happen if TargetActor is set to null by EndEncounter or similar logic.
        DemoLOG_CF(LogAI, Warning, TEXT("Neither TargetActor nor TargetLocation is set for %s"), *GetNameSafe(Pawn));
        return EStateTreeRunStatus::Failed;
    }

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

void USTTask_MoveTo::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    Super::ExitState(Context, Transition);

    if (AIController)
    {
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::HandleMoveCompleted);
    }
}

void USTTask_MoveTo::HandleMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
    if (AIController)
    {
        AIController->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::HandleMoveCompleted);
    }

    bool bSucceeded = Result == EPathFollowingResult::Success;
    FinishTask(bSucceeded);
}