// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_PerformAction.h"
#include "Components/StateManagerComponent.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/CombatInterface.h"
#include "StateTreeExecutionContext.h"

USTTask_PerformAction::USTTask_PerformAction(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShouldCallTick = false;
}

EStateTreeRunStatus USTTask_PerformAction::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    if (Super::EnterState(Context, Transition) == EStateTreeRunStatus::Failed || !Pawn.IsValid())
    {
        return EStateTreeRunStatus::Failed;
    }

    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Pawn);
    if (!CombatInterface)
    {
        return EStateTreeRunStatus::Failed;
    }

    const float Duration = CombatInterface->PerformAction(
        ActionTag,
        bIgnoreCurrentState,
        MontageIndex,
        bUseRandomIndex
    );
    if (Duration <= 0.f)
    {
        return EStateTreeRunStatus::Failed;
    }

    if (UStateManagerComponent* StateManager = Pawn->FindComponentByClass<UStateManagerComponent>())
    {
        StateEndedHandle = StateManager->OnStateEnded.AddUObject(this, &ThisClass::HandleStateEnded);
        return EStateTreeRunStatus::Running;
    }

    return EStateTreeRunStatus::Succeeded;

}

void USTTask_PerformAction::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
    Super::ExitState(Context, Transition);

    if (Pawn.IsValid())
    {
        if (UStateManagerComponent* StateManager = Pawn->FindComponentByClass<UStateManagerComponent>())
        {
            if (StateManager->OnStateEnded.Remove(StateEndedHandle))
            {
                StateEndedHandle.Reset();
            }
        }
    }
}

void USTTask_PerformAction::HandleStateEnded(FGameplayTag InState)
{
    if (Pawn.IsValid())
    {
        if (UStateManagerComponent* StateManager = Pawn->FindComponentByClass<UStateManagerComponent>())
        {
            if (StateManager->OnStateEnded.Remove(StateEndedHandle))
            {
                StateEndedHandle.Reset();
            }
        }
    }
    FinishTask(true);
}