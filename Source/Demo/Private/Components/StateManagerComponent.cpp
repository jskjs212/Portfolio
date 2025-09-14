// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoGameplayTags.h"

#include <mutex>

UStateManagerComponent::UStateManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetupAllowedTransitionsOnlyOnce();
}

void UStateManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentState = DemoGameplayTags::State_General;
    CurrentAction = DemoGameplayTags::State_General;
}

void UStateManagerComponent::SetAction(const FGameplayTag NewAction)
{
    if (CurrentAction == NewAction)
    {
        return;
    }

    FGameplayTag NewState = GetStateFromAction(NewAction);
    if (!NewState.IsValid())
    {
        return;
    }

    // @debug
    //UE_LOG(LogTemp, Display, TEXT("New state: %s, New action: %s"), *NewState.ToString(), *NewAction.ToString());

    // Set state and action
    SetState(NewState);
    CurrentAction = NewAction;
}

void UStateManagerComponent::SetState(const FGameplayTag NewState)
{
    if (CurrentState == NewState)
    {
        // Actions may change, but state is the same.
        return;
    }

    OnStateEnded.Broadcast(CurrentState);

    CurrentState = NewState;

    OnStateBegan.Broadcast(CurrentState);
}

void UStateManagerComponent::SetupAllowedTransitionsOnlyOnce()
{
    static std::once_flag bFlag;
    std::call_once(bFlag, []()
        {
            const FGameplayTagContainer& EmptyStates = FGameplayTagContainer::EmptyContainer;
            const FGameplayTagContainer AllStates = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
                DemoGameplayTags::State_General,
                    DemoGameplayTags::State_Dead,
                    DemoGameplayTags::State_Disabled,
                    DemoGameplayTags::State_Jump,
                    DemoGameplayTags::State_ToggleCombat,
                    DemoGameplayTags::State_Attack,
                    DemoGameplayTags::State_Dodge,
                    DemoGameplayTags::State_Block
            });
            const FGameplayTagContainer BasicStates = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
                DemoGameplayTags::State_General,
                    DemoGameplayTags::State_Dead,
                    DemoGameplayTags::State_Disabled
            });

            // General -> Any
            AllowedTransitions.Add(DemoGameplayTags::State_General, AllStates);

            // Dead or disabled -> None
            AllowedTransitions.Add(DemoGameplayTags::State_Dead, EmptyStates);
            AllowedTransitions.Add(DemoGameplayTags::State_Disabled, EmptyStates);

            // Others -> Basic
            AllowedTransitions.Add(DemoGameplayTags::State_Jump, BasicStates);
            AllowedTransitions.Add(DemoGameplayTags::State_ToggleCombat, BasicStates);
            AllowedTransitions.Add(DemoGameplayTags::State_Attack, BasicStates);
            AllowedTransitions.Add(DemoGameplayTags::State_Dodge, BasicStates);
            AllowedTransitions.Add(DemoGameplayTags::State_Block, BasicStates);

            // @TODO - Allow toggling combat when jumping?
            // @check - Allow canceling attacks by toggling combat or dodging?
            // @check - Allow canceling ToggleCombat_Exit by attacking or dodging?
            //          -> Currently it's meaningless to ToggleCombat_Exit, because there's no advantage to do so.

            // @TEST
            UE_LOG(LogTemp, Warning, TEXT("UStateManagerComponent::SetupAllowedTransitionsOnlyOnce"));
        });
}

bool UStateManagerComponent::CanPerformAction(const FGameplayTag NewAction) const
{
    const FGameplayTagContainer* const ToStates = AllowedTransitions.Find(CurrentState);

    return ToStates ? NewAction.MatchesAny(*ToStates) : false;
}

FGameplayTag UStateManagerComponent::GetStateFromAction(const FGameplayTag InAction) const
{
    FGameplayTag DerivedState = InAction;                                       // 'State.[state]'  or 'State.[state].[action]'
    const FGameplayTag ParentTag = InAction.RequestDirectParent();          // 'State'          or 'State.[state]'
    const FGameplayTag GrandParentTag = ParentTag.RequestDirectParent();    // EmptyTag         or 'State'

    // Check if GrandParentTag is 'State'
    if (GrandParentTag.MatchesTagExact(DemoGameplayTags::State))
    {
        DerivedState = ParentTag; // 'State.[state]'
    }
    // ParentTag is not 'State' or 'State.[state]' == invalid action input
    else if (!ParentTag.MatchesTagExact(DemoGameplayTags::State))
    {
        UE_LOG(LogTemp, Error, TEXT("UStateManagerComponent::SetAction: Invalid action tag %s"), *InAction.ToString());
        return FGameplayTag::EmptyTag;
    }

    return DerivedState;
}