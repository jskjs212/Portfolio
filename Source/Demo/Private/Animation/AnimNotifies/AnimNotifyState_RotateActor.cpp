// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimNotifies/AnimNotifyState_RotateActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/CombatInterface.h"

void UAnimNotifyState_RotateActor::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    static_assert(static_cast<uint8>(ERotateType::Count) == 2, "You added a new ERotateType value, but didn't update the switch statements.");

    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

    if (AActor* OwnerActor = MeshComp->GetOwner())
    {
        if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerActor))
        {
            const FRotator Current = OwnerActor->GetActorRotation();
            FRotator Target = FRotator::ZeroRotator;

            switch (RotateType)
            {
            case ERotateType::Control:
                Target = CombatInterface->GetDesiredControlRotation();
                break;

            case ERotateType::Input:
                Target = CombatInterface->GetDesiredInputRotation();
                break;

            default: _UNLIKELY
                checkNoEntry();
                break;
            }

            const FRotator NewRotation = FMath::RInterpTo(Current, Target, FrameDeltaTime, RotationInterpSpeed);
            OwnerActor->SetActorRotation(NewRotation);
        }
    }
}