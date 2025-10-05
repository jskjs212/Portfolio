// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_PerformAction.generated.h"

/**
 *
 */
UCLASS()
class DEMO_API UBTTask_PerformAction : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_PerformAction();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Categories = "State"))
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bIgnoreCurrentState{false};

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (ClampMin = "0"))
    int32 MontageIndex{0};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bUseRandomIndex{false};
};