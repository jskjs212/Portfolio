// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/CharacterMovementComponent.h"

// std library
#include <mutex>

UStateManagerComponent::UStateManagerComponent()
{
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
    DemoLOG_F(LogStateManager, Verbose, TEXT("Owner: %s, Action: %s"), *GetNameSafe(GetOwner()), *NewAction.ToString());

    // Set state and action
    SetState(NewState);
    CurrentAction = NewAction;
}

void UStateManagerComponent::SetDefaultAction()
{
    // Reset to Jump if the actor is falling
    if (AActor* OwnerActor = GetOwner())
    {
        if (UCharacterMovementComponent* MovementComponent = OwnerActor->FindComponentByClass<UCharacterMovementComponent>())
        {
            if (MovementComponent->IsFalling())
            {
                SetAction(DemoGameplayTags::State_Jump);
                return;
            }
        }
    }

    SetAction(DemoGameplayTags::State_General);
}

void UStateManagerComponent::OnLanded()
{
    // If already performing another action, then carry on.
    if (CurrentState == DemoGameplayTags::State_Jump)
    {
        SetAction(DemoGameplayTags::State_General);
    }
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

            // @check - Allow canceling attacks by toggling combat or dodging?
            // @check - Allow canceling ToggleCombat_Exit by attacking or dodging?
            //          -> Currently it's meaningless to ToggleCombat_Exit, because there's no advantage to do so.
        });
}

bool UStateManagerComponent::CanMoveInCurrentState() const
{
    // @hardcoded
    static const FGameplayTagContainer MovementBlockingStates = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
        DemoGameplayTags::State_Dead,
            DemoGameplayTags::State_Disabled,
            DemoGameplayTags::State_Attack,
            DemoGameplayTags::State_Dodge
    });
    // Blocked states, but allow specific actions.
    static const FGameplayTagContainer MovementAllowingActions = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
        //DemoGameplayTags::State_Disabled_HitstunFront,
        //    DemoGameplayTags::State_Disabled_HitstunBack,
        //    DemoGameplayTags::State_Disabled_HitstunLeft,
        //    DemoGameplayTags::State_Disabled_HitstunRight
    });
    return !CurrentState.MatchesAnyExact(MovementBlockingStates) || CurrentAction.MatchesAnyExact(MovementAllowingActions);
}

bool UStateManagerComponent::CanChangeEquipment() const
{
    // @hardcoded
    static const FGameplayTagContainer ChangeEquipmentBlockingStates = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
        DemoGameplayTags::State_Dead,
            DemoGameplayTags::State_Disabled,
            DemoGameplayTags::State_Attack
    });
    return !CurrentState.MatchesAnyExact(ChangeEquipmentBlockingStates);
}

bool UStateManagerComponent::CanPerformAction(const FGameplayTag NewAction) const
{
    const FGameplayTagContainer* const ToStates = AllowedTransitions.Find(CurrentState);
    checkf(ToStates, TEXT("Not even initialized. Owner: %s"), *GetNameSafe(GetOwner()));
    return GetStateFromAction(NewAction).MatchesAnyExact(*ToStates);
}

FGameplayTag UStateManagerComponent::GetStateFromAction(const FGameplayTag InAction) const
{
    //                                                  If InAction is one of: 'State.[state]'  or 'State.[state].[action]'
    const FGameplayTag ParentTag = InAction.RequestDirectParent();          // 'State'          or 'State.[state]'
    const FGameplayTag GrandParentTag = ParentTag.RequestDirectParent();    // EmptyTag         or 'State'

    // Check if GrandParentTag is 'State'
    if (GrandParentTag.MatchesTagExact(DemoGameplayTags::State))
    {
        return ParentTag; // 'State.[state]'
    }

    // ParentTag is not 'State' or 'State.[state]' == invalid action input
    if (!ParentTag.MatchesTagExact(DemoGameplayTags::State))
    {
        DemoLOG_CF(LogStateManager, Error, TEXT("Invalid action tag %s"), *InAction.ToString());
        return FGameplayTag::EmptyTag;
    }

    return InAction;
}