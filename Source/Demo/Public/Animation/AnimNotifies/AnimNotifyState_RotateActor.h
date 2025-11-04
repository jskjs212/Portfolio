// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_RotateActor.generated.h"

UENUM()
enum class ERotateType : uint8
{
    Control UMETA(DisplayName = "Control (camera)"),
    Input UMETA(DisplayName = "Input (character)"),
    Count UMETA(Hidden)
};

/**
 * Rotate the ICombatInterface actor to face the desired rotation.
 */
UCLASS(meta = (DisplayName = "Rotate Actor"))
class DEMO_API UAnimNotifyState_RotateActor : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    ERotateType RotateType{ERotateType::Control};

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Min = "0."))
    float RotationInterpSpeed{10.f};
};