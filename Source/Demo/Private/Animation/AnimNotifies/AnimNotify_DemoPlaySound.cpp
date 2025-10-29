// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotify_DemoPlaySound.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_DemoPlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    UWorld* World = MeshComp->GetWorld();
    if (!World)
    {
        return;
    }

    if (!SoundTag.IsValid())
    {
        DemoLOG_CF(LogAnimation, Warning, TEXT("Sound tag is not set. Animation: %s"), *GetNameSafe(Animation));
        return;
    }

#if WITH_EDITORONLY_DATA
    if (World->WorldType == EWorldType::EditorPreview)
    {
        if (MeshComp->IsPlaying())
        {
            UGameplayStatics::PlaySound2D(World, EditorPreviewSound, EditorPreviewVolumeMultiplier, EditorPreviewPitchMultiplier);
        }
    }
    else
#endif // WITH_EDITORONLY_DATA
    {
        if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(World->GetGameInstance()))
        {
            if (bFollow)
            {
                AudioSubsystem->PlaySoundAttached(SoundTag, MeshComp, AttachName, FVector{ForceInit}, EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier);
            }
            else
            {
                AudioSubsystem->PlaySoundAtLocation(World, SoundTag, MeshComp->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
            }
        }
    }
}