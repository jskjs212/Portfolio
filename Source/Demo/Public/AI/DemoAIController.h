// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "DemoAIController.generated.h"

//struct FActorPerceptionUpdateInfo;
struct FStateTreeReference;
class UAIPerceptionComponent;
class UAISenseConfig_Damage;
class UAISenseConfig_Sight;
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
    // Default StateTree is an empty tree with only one state that can be overridden with the same StateTreeTag.
    // If the possessed pawn is an AICharacter and has a valid StateTreeRefOverride, it will be used instead and execute StartLogic().
    // StateTreeAIComponent->SetStateTree() has problem transferring the parameters.
    // StateTreeAIComponent->SetStateTreeReference() shows error message from FRuntimeValidationInstanceData::SetContext.
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UDemoStateTreeAIComponent> DemoStateTreeAIComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAIPerceptionComponent> DemoPerceptionComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAISenseConfig_Damage> SenseDamageConfig;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAISenseConfig_Sight> SenseSightConfig;

    ////////////////////////////////////////////////////////
    //        AIController
    ////////////////////////////////////////////////////////
public:
    ADemoAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

public:
    virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr) override;

    void SetControlRotationToTarget();

    void SetIsBoss(bool bInIsBoss) { bIsBoss = bInIsBoss; }

protected:
    void OverrideStateTree(FStateTreeReference InStateTreeRef);

    void OverrideStateTree(const APawn* InPawn);

    UFUNCTION()
    void HandleTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }

    // Ignores perception system, directly sets the target actor.
    void SetTargetActor(AActor* InTargetActor);

    ////////////////////////////////////////////////////////
    //        StateTree variables (like Blackboard keys)
    ////////////////////////////////////////////////////////
protected:
    // Target actor that AI is attacking or moving to.
    TObjectPtr<AActor> TargetActor;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    bool bIsBoss{false};

    UPROPERTY(EditAnywhere, Category = "Initialization|AI|StateTree")
    FGameplayTag StateTreeTag;
};
