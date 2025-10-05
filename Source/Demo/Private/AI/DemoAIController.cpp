// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DemoAIController.h"
#include "Character/AICharacter.h"
#include "Perception/AIPerceptionComponent.h"

ADemoAIController::ADemoAIController()
{
    DemoPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*DemoPerceptionComponent);
}

void ADemoAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
}