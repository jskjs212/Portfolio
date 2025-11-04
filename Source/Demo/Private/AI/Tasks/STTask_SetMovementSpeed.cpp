// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_SetMovementSpeed.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StateTreeExecutionContext.h"

FSTTask_SetMovementSpeed::FSTTask_SetMovementSpeed()
{
    bShouldCallTick = false;
}

EStateTreeRunStatus FSTTask_SetMovementSpeed::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    if (InstanceData.Character)
    {
        if (UCharacterMovementComponent* MovementComponent = InstanceData.Character->GetCharacterMovement())
        {
            MovementComponent->MaxWalkSpeed = InstanceData.NewMaxWalkSpeed;
            return EStateTreeRunStatus::Succeeded;
        }
    }
    return EStateTreeRunStatus::Failed;
}