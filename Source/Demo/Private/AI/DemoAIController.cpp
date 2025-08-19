// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DemoAIController.h"
#include "Character/AICharacter.h"

void ADemoAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (GetPawn<AAICharacter>())
    {
        RunBehaviorTree(GetBehaviorTreeToRun());
    }
}

UBehaviorTree* ADemoAIController::GetBehaviorTreeToRun() const
{
    if (AAICharacter* AICharacter = GetPawn<AAICharacter>())
    {
        if (UBehaviorTree* BehaviorTree = AICharacter->GetBehaviorTreeOverride())
        {
            return BehaviorTree;
        }
    }
    return DefaultBehaviorTree;
}