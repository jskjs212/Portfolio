// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_GetTargetActor.h"
#include "AI/DemoAIController.h"
#include "DemoTypes/LogCategories.h"
#include "StateTreeExecutionContext.h"

FSTTask_GetTargetActor::FSTTask_GetTargetActor()
{
    bShouldCallTick = false;
}

EStateTreeRunStatus FSTTask_GetTargetActor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    if (!InstanceData.DemoAIController)
    {
        DemoLOG_CF(LogAI, Error, TEXT("DemoAIController is not valid."));
        return EStateTreeRunStatus::Failed;
    }

    InstanceData.TargetActor = InstanceData.DemoAIController->GetTargetActor();

    return EStateTreeRunStatus::Succeeded;
}