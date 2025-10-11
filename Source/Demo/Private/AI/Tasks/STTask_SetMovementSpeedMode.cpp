// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_SetMovementSpeedMode.h"
#include "Character/BaseCharacter.h"
#include "DemoTypes/LogCategories.h"
#include "StateTreeExecutionContext.h"

FSTTask_SetMovementSpeedMode::FSTTask_SetMovementSpeedMode()
{
    bShouldCallTick = false;
}

EStateTreeRunStatus FSTTask_SetMovementSpeedMode::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    if (!InstanceData.BaseCharacter)
    {
        DemoLOG_CF(LogAI, Error, TEXT("BaseCharacter is not valid."));
        return EStateTreeRunStatus::Failed;
    }

    InstanceData.BaseCharacter->SetMovementSpeedMode(InstanceData.NewSpeedMode);

    return EStateTreeRunStatus::Succeeded;
}