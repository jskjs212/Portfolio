// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "STTask_PlaySound.generated.h"

USTRUCT()
struct FSTTask_PlaySoundInstanceData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Context")
    TObjectPtr<ACharacter> Character;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    FGameplayTag SoundTag;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    float VolumeMultiplier{1.f};

    UPROPERTY(EditAnywhere, Category = "Parameter")
    float PitchMultiplier{1.f};

    UPROPERTY(EditAnywhere, Category = "Parameter")
    bool bFollow{false};

    UPROPERTY(EditAnywhere, Category = "Parameter")
    FName AttachName;
};

/**
 *
 */
USTRUCT(meta = (DisplayName = "Play Sound", Category = "Demo|Common"))
struct DEMO_API FSTTask_PlaySound : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FSTTask_PlaySoundInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    };

    FSTTask_PlaySound();

    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};