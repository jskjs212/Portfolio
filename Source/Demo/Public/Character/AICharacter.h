// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interfaces/TargetInterface.h"
#include "AICharacter.generated.h"

class UAIStatusWidget;
class UBehaviorTree;
class UWidgetComponent;

/**
 * AICharacter class that should be inherited by enemy or NPC character classes.
 */
UCLASS()
class DEMO_API AAICharacter : public ABaseCharacter, public ITargetInterface
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(VisibleAnywhere, Category = "AI")
    TObjectPtr<UWidgetComponent> WidgetComponent;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    AAICharacter();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Target interface
    ////////////////////////////////////////////////////////
public:
    virtual bool CanBeTargeted() const override;

    virtual void OnTargeted(bool bIsTargeted) override;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE UBehaviorTree* GetBehaviorTreeOverride() const { return BehaviorTreeOverride; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "Initialization|AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeOverride;
};