// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_FindNextTargetLocation.h"
#include "DemoTypes/LogCategories.h"
#include "Engine/TargetPoint.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus USTTask_FindNextTargetLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (Super::EnterState(Context, Transition) == EStateTreeRunStatus::Failed)
    {
        return EStateTreeRunStatus::Failed;
    }

    if (TargetPoints.IsEmpty())
    {
        DemoLOG_CF(LogAI, Warning, TEXT("Target points are empty for %s"), *GetNameSafe(Actor));
        return EStateTreeRunStatus::Failed;
    }

    int32 NewIndex = CurrentTargetIndex + 1;
    if (NewIndex >= TargetPoints.Num())
    {
        NewIndex = 0;
    }

    const ATargetPoint* TargetPoint = TargetPoints[NewIndex];
    if (!TargetPoint)
    {
        DemoLOG_CF(LogAI, Warning, TEXT("Target point is null for %s (index: %d)"), *GetNameSafe(Actor), NewIndex);
        return EStateTreeRunStatus::Failed;
    }

    CurrentTargetIndex = NewIndex;
    TargetLocation = TargetPoint->GetActorLocation();

    return EStateTreeRunStatus::Succeeded;
}