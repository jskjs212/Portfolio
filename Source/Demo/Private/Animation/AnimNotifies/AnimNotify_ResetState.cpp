// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotify_ResetState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StateManagerComponent.h"

void UAnimNotify_ResetState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (const AActor* OwnerActor = MeshComp->GetOwner())
    {
        if (UStateManagerComponent* StateManager = OwnerActor->FindComponentByClass<UStateManagerComponent>())
        {
            StateManager->SetDefaultAction();
        }
    }
}