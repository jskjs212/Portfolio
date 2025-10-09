// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_ContinueAttack.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Continue Attack"))
class DEMO_API UAnimNotify_ContinueAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Categories = "State.Attack"))
    FGameplayTag AttackAction;
};