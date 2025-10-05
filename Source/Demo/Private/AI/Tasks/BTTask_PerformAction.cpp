// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_PerformAction.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Interfaces/CombatInterface.h"

UBTTask_PerformAction::UBTTask_PerformAction()
{
    NodeName = TEXT("Perform Action");
}

EBTNodeResult::Type UBTTask_PerformAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (Super::ExecuteTask(OwnerComp, NodeMemory) == EBTNodeResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    const AAIController* AIController = OwnerComp.GetAIOwner();
    ICombatInterface* CombatInterface = AIController ? AIController->GetPawn<ICombatInterface>() : nullptr;
    if (!CombatInterface)
    {
        return EBTNodeResult::Failed;
    }

    const float Duration = CombatInterface->PerformAction(ActionTag, bIgnoreCurrentState, MontageIndex, bUseRandomIndex);

    return Duration > 0.f ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}