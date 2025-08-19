// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AICharacter.h"

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!BehaviorTreeOverride)
    {
        UE_LOG(LogTemp, Warning, TEXT("This AICharacter (%s) is using default BehaviorTree."), *GetName());
    }
}