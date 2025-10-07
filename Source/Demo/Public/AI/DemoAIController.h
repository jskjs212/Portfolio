// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "DemoAIController.generated.h"

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
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UAIPerceptionComponent> DemoPerceptionComponent;

    // Default StateTree is an empty tree with only one state that can be overridden with the same StateTreeTag.
    // If the possessed pawn is an AICharacter and has a valid StateTreeRefOverride, it will be used instead and execute StartLogic().
    // StateTreeAIComponent->SetStateTree() has problem transferring the parameters.
    // StateTreeAIComponent->SetStateTreeReference() shows error message from FRuntimeValidationInstanceData::SetContext.
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UDemoStateTreeAIComponent> DemoStateTreeAIComponent;

    ////////////////////////////////////////////////////////
    //        AIController
    ////////////////////////////////////////////////////////
public:
    ADemoAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "Initialization|AI|StateTree")
    FGameplayTag StateTreeTag;
};
