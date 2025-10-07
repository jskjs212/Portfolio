// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_FindNextTargetLocation.h"
#include "DemoTypes/LogCategories.h"
#include "Engine/TargetPoint.h"
#include "StateTreeExecutionContext.h"

FSTTask_FindNextTargetLocation::FSTTask_FindNextTargetLocation()
{
    bShouldStateChangeOnReselect = false;
    bShouldCallTick = false;
}

EStateTreeRunStatus FSTTask_FindNextTargetLocation::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    // Get mutable references to the properties (previous index and target points array)
    int32* PreviousIndexPtr = InstanceData.RefToPreviousTargetIndex.GetMutablePtr(Context);
    if (!PreviousIndexPtr)
    {
        DemoLOG_CF(LogAI, Error, TEXT("Failed to get previous target index for %s"), *GetNameSafe(InstanceData.Pawn));
        return EStateTreeRunStatus::Failed;
    }
    const TArray<ATargetPoint*>* TargetPointsPtr = InstanceData.RefToArrayOfTargetPoints.GetMutablePtr(Context);
    if (!TargetPointsPtr)
    {
        DemoLOG_CF(LogAI, Error, TEXT("Failed to get target points array for %s"), *GetNameSafe(InstanceData.Pawn));
        return EStateTreeRunStatus::Failed;
    }

    // Validate the array
    if (TargetPointsPtr->IsEmpty())
    {
        DemoLOG_CF(LogAI, Error, TEXT("Target points are empty for %s"), *GetNameSafe(InstanceData.Pawn));
        return EStateTreeRunStatus::Failed;
    }

    // Calculate the current index
    int32 CurrentIndex = *PreviousIndexPtr + 1;
    if (CurrentIndex >= TargetPointsPtr->Num())
    {
        CurrentIndex = 0;
    }

    // Get the target point
    const ATargetPoint* TargetPoint = (*TargetPointsPtr)[CurrentIndex];
    if (!TargetPoint)
    {
        DemoLOG_CF(LogAI, Error, TEXT("Target point is null for %s (index: %d)"), *GetNameSafe(InstanceData.Pawn), CurrentIndex);
        return EStateTreeRunStatus::Failed;
    }

    // Update
    *PreviousIndexPtr = CurrentIndex;
    InstanceData.TargetLocation = TargetPoint->GetActorLocation();

    return EStateTreeRunStatus::Succeeded;
}