// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "DemoAIController.generated.h"

struct FActorPerceptionUpdateInfo;
struct FStateTreeReference;
class UAIPerceptionComponent;
class UDemoStateTreeAIComponent;

/**
 *
 */
UCLASS()
class DEMO_API ADemoAIController : public AAIController
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    // Default StateTree is an empty tree with two linked assets (Peaceful and Aggressive) that can be overridden.
    // If the possessed pawn is an AICharacter and has a valid StateTreeRefOverride, it will be used instead and execute StartLogic().
    // StateTreeAIComponent->SetStateTree() has problem transferring the parameters.
    // StateTreeAIComponent->SetStateTreeReference() shows error message from FRuntimeValidationInstanceData::SetContext.
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UDemoStateTreeAIComponent> DemoStateTreeAIComponent;

    // Sense: Sight, damage
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAIPerceptionComponent> DemoPerceptionComponent;

    ////////////////////////////////////////////////////////
    //        AIController functions
    ////////////////////////////////////////////////////////
public:
    ADemoAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

public:
    // If the current state disallows movement, deny the request for AAIController::MoveTo.
    virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr) override;

    void SendStateTreeEvent(FGameplayTag InTag, FName InOrigin);

protected:
    void OverrideStateTree(FStateTreeReference InStateTreeRef, FGameplayTag InStateTreeTag);

    void OverrideStateTree(const APawn* InPawn);

    UFUNCTION()
    void HandleTargetPerceptionUpdated(AActor* InActor, FAIStimulus Stimulus);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE AActor* GetTargetActor() const { return TargetActor.Get(); }

    void SetIsBoss(bool bInIsBoss) { bIsBoss = bInIsBoss; }

    // Ignores perception system, directly sets the target actor.
    void SetTargetActorManually(AActor* InTargetActor);

    void SetControlRotationToTarget();

    ////////////////////////////////////////////////////////
    //        StateTree variables (like Blackboard keys)
    ////////////////////////////////////////////////////////
protected:
    // Target actor that AI is attacking or moving to.
    TWeakObjectPtr<AActor> TargetActor;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    bool bIsBoss{false};

    UPROPERTY(EditAnywhere, Category = "Initialization|AI|StateTree")
    FGameplayTag PeacefulStateTreeTag;

    UPROPERTY(EditAnywhere, Category = "Initialization|AI|StateTree")
    FGameplayTag AggressiveStateTreeTag;
};
