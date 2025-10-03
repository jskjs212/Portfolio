// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotifyState_CollisionTrace.h"
#include "Components/CollisionComponent.h"

void UAnimNotifyState_CollisionTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (AActor* OwnerActor = MeshComp->GetOwner())
    {
        if (UCollisionComponent* CollisionComponent = OwnerActor->FindComponentByClass<UCollisionComponent>())
        {
            CollisionComponent->ActivateCollisionDefinition(AttackCollisionType, HitGroup, bClearHitActorsOnBegin);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("AnimNotifyState_CollisionTrace: %s has no CollisionComponent."), *OwnerActor->GetName());
        }
    }
}

void UAnimNotifyState_CollisionTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (AActor* OwnerActor = MeshComp->GetOwner())
    {
        if (UCollisionComponent* CollisionComponent = OwnerActor->FindComponentByClass<UCollisionComponent>())
        {
            CollisionComponent->DeactivateCollisionDefinition(AttackCollisionType, HitGroup);
        }
    }
}