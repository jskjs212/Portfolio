// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DemoAIController.generated.h"

class UBehaviorTree;

/**
 *
 */
UCLASS()
class DEMO_API ADemoAIController : public AAIController
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        AIController
    ////////////////////////////////////////////////////////
protected:
    virtual void OnPossess(APawn* InPawn) override;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
protected:
    // Overrides the default BT if it exists
    UBehaviorTree* GetBehaviorTreeToRun() const;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    TObjectPtr<UBehaviorTree> DefaultBehaviorTree;
};
