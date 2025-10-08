// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Conditions/STCondition_CanMove.h"
#include "Components/StateManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "StateTreeExecutionContext.h"

bool FSTCondition_CanMove::TestCondition(FStateTreeExecutionContext& Context) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    if (InstanceData.Pawn)
    {
        if (UStateManagerComponent* StateManager = InstanceData.Pawn->FindComponentByClass<UStateManagerComponent>())
        {
            return StateManager->CanMoveInCurrentState();
        }
    }
    return false;
}