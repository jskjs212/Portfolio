// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CombatComponent.h"
#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Interfaces/CombatInterface.h"

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* OwnerActor = GetOwner())
    {
        if (UStateManagerComponent* StateManager = OwnerActor->FindComponentByClass<UStateManagerComponent>())
        {
            StateManager->OnStateEnded.AddUObject(this, &UCombatComponent::HandleStateEnded);
        }
    }
}

float UCombatComponent::ToggleCombat()
{
    // @TODO

    return 0.f;
}

float UCombatComponent::Attack(FGameplayTag AttackAction)
{
    ICombatInterface* CombatInterface = GetOwner<ICombatInterface>();
    if (!CombatInterface)
    {
        return 0.f;
    }

    if (!IsCombatReady())
    {
        return ToggleCombat();
    }

    // Buffer next attack
    if (!bIsAttackBuffered && CombatInterface->IsInAction(AttackAction))
    {
        bIsAttackBuffered = true;
        ++AttackIndex;
        if (AttackIndex >= CombatInterface->GetActionInfoCount(AttackAction))
        {
            AttackIndex = 0;
        }
        return 0.f;
    }

    // Attack
    CurrentActionIndex = AttackIndex;
    return CombatInterface->PerformAction(AttackAction, false, AttackIndex);
}

float UCombatComponent::ContinueAttack(FGameplayTag AttackAction)
{
    if (bIsAttackBuffered)
    {
        bIsAttackBuffered = false;

        // Infinite combo not allowed
        if (AttackIndex != 0)
        {
            if (ICombatInterface* CombatInterface = GetOwner<ICombatInterface>())
            {
                CurrentActionIndex = AttackIndex;
                return CombatInterface->PerformAction(AttackAction, true, AttackIndex);
            }
        }
    }
    return 0.f;
}

void UCombatComponent::ResetCombat()
{
    bIsAttackBuffered = false;
    CurrentActionIndex = 0;
    AttackIndex = 0;
}

void UCombatComponent::HandleStateEnded(FGameplayTag InState)
{
    if (InState == DemoGameplayTags::State_Attack)
    {
        ResetCombat();
    }
}