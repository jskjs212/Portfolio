// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotify_ContinueAttack.h"
#include "Components/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_ContinueAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!AttackAction.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("UAnimNotify_ContinueAttack - AttackAction tag is not set."));
        return;
    }

    if (const AActor* OwnerActor = MeshComp->GetOwner())
    {
        if (UCombatComponent* CombatComponent = OwnerActor->FindComponentByClass<UCombatComponent>())
        {
            CombatComponent->ContinueAttack(AttackAction);
        }
    }
}