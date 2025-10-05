// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_PerformAction.h"
#include "Interfaces/CombatInterface.h"

EStateTreeRunStatus USTTask_PerformAction::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (Super::EnterState(Context, Transition) == EStateTreeRunStatus::Failed)
    {
        return EStateTreeRunStatus::Failed;
    }

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor);
    if (!CombatInterface)
    {
        return EStateTreeRunStatus::Failed;
    }

    const float Duration = CombatInterface->PerformAction(ActionTag, bIgnoreCurrentState, MontageIndex, bUseRandomIndex);

    return Duration > 0.f ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
}