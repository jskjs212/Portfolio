// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotify_ContinueAttack.h"
#include "Components/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DemoTypes/LogCategories.h"

void UAnimNotify_ContinueAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!AttackAction.IsValid())
    {
        DemoLOG_CF(LogAnimation, Error, TEXT("AttackAction tag is not valid."));
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