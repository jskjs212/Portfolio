// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "StateManagerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStateManager, Log, All);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, FGameplayTag /* InState */);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UStateManagerComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnStateChanged OnStateBegan;
    FOnStateChanged OnStateEnded;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UStateManagerComponent();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        State manager functions
    ////////////////////////////////////////////////////////
public:
    // Set action tag and state tag.
    // State tag will be set to 'State.[state]'.
    // @param NewAction should be 'State.[state]' or 'State.[state].[action]'
    void SetAction(FGameplayTag NewAction);

    // Reset to General or Jump (if falling).
    void SetDefaultAction();

    // Reset state when landed.
    void OnLanded();

private:
    // Set only state tag.
    void SetState(FGameplayTag NewState);

    void SetupAllowedTransitionsOnlyOnce();

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE bool IsInState(FGameplayTag State) const { return CurrentState == State; }

    // Determine if the current state allows performing the NewAction.
    // Only state and action are considered, other conditions like character's immune state are not.
    bool CanPerformAction(FGameplayTag NewAction) const;

    FGameplayTag GetStateFromAction(FGameplayTag InAction) const;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    // Current state: Jump, attack
    UPROPERTY(VisibleAnywhere, Category = "State", meta = (Categories = "State"))
    FGameplayTag CurrentState;

    // Current action (specific state): Light attack
    UPROPERTY(VisibleAnywhere, Category = "State", meta = (Categories = "State"))
    FGameplayTag CurrentAction;

    // { FromState, ToStates }
    // FromState can transition to any of ToStates.
    static inline TMap<FGameplayTag, FGameplayTagContainer> AllowedTransitions{};
};