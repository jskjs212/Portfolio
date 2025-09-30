// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DemoTypes/AttackTypes.h"
#include "AnimNotifyState_CollisionTrace.generated.h"

/**
 *
 */
UCLASS()
class DEMO_API UAnimNotifyState_CollisionTrace : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    EAttackCollisionType CollisionType{EAttackCollisionType::None};
};