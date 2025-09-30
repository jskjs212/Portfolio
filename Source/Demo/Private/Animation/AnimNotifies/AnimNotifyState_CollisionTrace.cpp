// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotifyState_CollisionTrace.h"
#include "Interfaces/CombatInterface.h"

void UAnimNotifyState_CollisionTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner()))
    {
        CombatInterface->SetAttackCollisionEnabled(CollisionType, true);
    }
}

void UAnimNotifyState_CollisionTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MeshComp->GetOwner()))
    {
        CombatInterface->SetAttackCollisionEnabled(CollisionType, false);
    }
}