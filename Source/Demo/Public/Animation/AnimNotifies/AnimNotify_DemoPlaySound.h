// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_DemoPlaySound.generated.h"

/**
 *
 */
UCLASS(meta = (DisplayName = "Demo Play Sound"))
class DEMO_API UAnimNotify_DemoPlaySound : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (Categories = "Sound"))
    FGameplayTag SoundTag;

    UPROPERTY(EditAnywhere, Category = "AnimNotify")
    float VolumeMultiplier{1.f};

    UPROPERTY(EditAnywhere, Category = "AnimNotify")
    float PitchMultiplier{1.f};

    // Follow its owner
    UPROPERTY(EditAnywhere, Category = "AnimNotify")
    bool bFollow{false};

    // Socket name to attach sound to
    UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (EditCondition = "bFollow"))
    FName AttachName;

#if WITH_EDITORONLY_DATA
    // Just for previewing in editor.
    // In runtime, sound will be found by SoundTag from SoundCollection data asset.
    UPROPERTY(EditAnywhere, Category = "AnimNotify|Editor")
    TObjectPtr<USoundBase> EditorPreviewSound;

    UPROPERTY(EditAnywhere, Category = "AnimNotify|Editor")
    float EditorPreviewVolumeMultiplier{0.25f};

    UPROPERTY(EditAnywhere, Category = "AnimNotify|Editor")
    float EditorPreviewPitchMultiplier{1.f};
#endif // WITH_EDITORONLY_DATA
};