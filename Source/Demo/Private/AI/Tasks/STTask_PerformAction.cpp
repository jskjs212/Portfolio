// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_PerformAction.h"
#include "Interfaces/CombatInterface.h"
#include "GameFramework/Pawn.h"
#include "StateTreeExecutionContext.h"

FSTTask_PerformAction::FSTTask_PerformAction()
{
    bShouldStateChangeOnReselect = false;
    bShouldCallTick = false;
}

EStateTreeRunStatus FSTTask_PerformAction::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(InstanceData.Pawn);
    if (!CombatInterface)
    {
        return EStateTreeRunStatus::Failed;
    }

    const float Duration = CombatInterface->PerformAction(
        InstanceData.ActionTag,
        InstanceData.bIgnoreCurrentState,
        InstanceData.MontageIndex,
        InstanceData.bUseRandomIndex
    );

    return Duration > 0.f ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
}