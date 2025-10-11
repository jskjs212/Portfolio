// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STTask_GetTargetActor.generated.h"

class ADemoAIController;

USTRUCT(BlueprintType)
struct FSTTask_GetTargetActorInstanceData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<ADemoAIController> DemoAIController;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Output")
    TObjectPtr<AActor> TargetActor;
};

/**
 *
 */
USTRUCT(meta = (DisplayName = "Get Target Actor", Category = "Demo|Common"))
struct DEMO_API FSTTask_GetTargetActor : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_GetTargetActorInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    FSTTask_GetTargetActor();

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};