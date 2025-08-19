// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "AICharacter.generated.h"

class UBehaviorTree;

/**
 *
 */
UCLASS()
class DEMO_API AAICharacter : public ABaseCharacter
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Fundametals
    ////////////////////////////////////////////////////////
public:
    AAICharacter() {}

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE UBehaviorTree* GetBehaviorTreeOverride() const { return BehaviorTreeOverride; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeOverride;
};