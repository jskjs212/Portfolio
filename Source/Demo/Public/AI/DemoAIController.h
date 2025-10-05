// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DemoAIController.generated.h"

class UAIPerceptionComponent;

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

    ////////////////////////////////////////////////////////
    //        AIController
    ////////////////////////////////////////////////////////
public:
    ADemoAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;
};
