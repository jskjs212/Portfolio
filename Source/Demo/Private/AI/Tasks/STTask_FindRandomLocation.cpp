// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_FindRandomLocation.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"

FSTTask_FindRandomLocation::FSTTask_FindRandomLocation()
{
    bShouldCallTick = false;
}

EStateTreeRunStatus FSTTask_FindRandomLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    if (!InstanceData.Pawn)
    {
        return EStateTreeRunStatus::Failed;
    }

    const FVector Origin = InstanceData.Pawn->GetActorLocation();

    bool bResult = UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(InstanceData.Pawn, Origin, InstanceData.RandomLocation, InstanceData.SearchRadius);

    return bResult ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
}