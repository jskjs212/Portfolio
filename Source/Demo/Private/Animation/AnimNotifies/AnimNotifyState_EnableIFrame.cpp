// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotifyState_EnableIFrame.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/CombatInterface.h"

void UAnimNotifyState_EnableIFrame::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (ICombatInterface* CombatInterface = MeshComp->GetOwner<ICombatInterface>())
    {
        CombatInterface->EnableIFrame(true);
    }
}

void UAnimNotifyState_EnableIFrame::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (ICombatInterface* CombatInterface = MeshComp->GetOwner<ICombatInterface>())
    {
        CombatInterface->EnableIFrame(false);
    }
}