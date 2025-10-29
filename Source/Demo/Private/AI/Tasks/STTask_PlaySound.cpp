// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/STTask_PlaySound.h"
#include "Audio/DemoAudioSubsystem.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Character.h"
#include "StateTreeExecutionContext.h"

FSTTask_PlaySound::FSTTask_PlaySound()
{
    bShouldCallTick = false;
}

EStateTreeRunStatus FSTTask_PlaySound::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    if (!InstanceData.Character)
    {
        return EStateTreeRunStatus::Failed;
    }

    if (!InstanceData.SoundTag.IsValid())
    {
        DemoLOG_CF(LogAI, Warning, TEXT("Sound tag is not set. Character: %s"), *InstanceData.Character->GetName());
        return EStateTreeRunStatus::Failed;
    }

    UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(InstanceData.Character->GetGameInstance());
    if (!AudioSubsystem)
    {
        return EStateTreeRunStatus::Failed;
    }

    if (InstanceData.bFollow)
    {
        AudioSubsystem->PlaySoundAttached(InstanceData.SoundTag, InstanceData.Character->GetMesh(), InstanceData.AttachName, FVector{ForceInit}, EAttachLocation::SnapToTarget, false, InstanceData.VolumeMultiplier, InstanceData.PitchMultiplier);
    }
    else
    {
        AudioSubsystem->PlaySoundAtLocation(InstanceData.Character, InstanceData.SoundTag, InstanceData.Character->GetActorLocation(), InstanceData.VolumeMultiplier, InstanceData.PitchMultiplier);
    }

    return EStateTreeRunStatus::Succeeded;
}