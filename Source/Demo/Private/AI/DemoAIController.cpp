// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DemoAIController.h"
#include "Character/AICharacter.h"
#include "Components/DemoStateTreeAIComponent.h"
#include "DemoTypes/LogCategories.h"
#include "Perception/AIPerceptionComponent.h"
#include "StateTreeReference.h"

ADemoAIController::ADemoAIController()
{
    DemoPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*DemoPerceptionComponent);

    DemoStateTreeAIComponent = CreateDefaultSubobject<UDemoStateTreeAIComponent>(TEXT("DemoStateTreeAIComponent"));
    DemoStateTreeAIComponent->SetStartLogicAutomatically(false);
}

void ADemoAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (const AAICharacter* AICharacter = Cast<AAICharacter>(InPawn))
    {
        if (DemoStateTreeAIComponent->IsRunning())
        {
            DemoStateTreeAIComponent->StopLogic(TEXT("Changing StateTreeRefOverride."));
        }

        FStateTreeReference StateTreeRefOverride = AICharacter->GetStateTreeRefOverride(); // Must be a copy.
        if (StateTreeRefOverride.IsValid())
        {
            DemoStateTreeAIComponent->RemoveLinkedStateTreeOverrides(StateTreeTag);
            DemoStateTreeAIComponent->AddLinkedStateTreeOverrides_Fixed(StateTreeTag, StateTreeRefOverride);
            DemoStateTreeAIComponent->StartLogic();
        }
        else
        {
            DemoLOG_CF(LogAI, Verbose, TEXT("%s has no valid StateTreeRefOverride. Stop logic."), *AICharacter->GetName());
        }
    }
    else
    {
        DemoLOG_CF(LogAI, Warning, TEXT("Possessed pawn (%s) is not an AAICharacter."), *GetNameSafe(InPawn));
    }
}