// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DemoTypes/AttackTypes.h"
#include "AnimNotifyState_CollisionTrace.generated.h"

/**
 * Activate attack collision of a certain type (definition) during the notify duration.
 * Same type and same hit group at the same time will be deactivated at the end of the first notify.
 */
UCLASS()
class DEMO_API UAnimNotifyState_CollisionTrace : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    // The type of attack collision definition to activate.
    // Multiple notifies with separate hit groups (hit multiple times at once) are not supported for now. @TODO
    UPROPERTY(EditAnywhere, Category = "Initialization")
    EAttackCollisionType AttackCollisionType{EAttackCollisionType::None};

    // !!! This must be true for first notify of the same type in a montage, because hit actors are not cleared on end.
    // true: Allows multiple hits on the same actor.
    // false: Allows one unique hit when attacking multiple times.
    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bClearHitActorsOnBegin{true};

    // !!! The array of TSet will grow as needed, and persist for the lifetime of the component.
    // Differentiates hit groups (TSet<AActor*> HitActors).
    // Same group: Allows one unique hit for different types.
    // Different group: Allows multiple hits on the same actor.
    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (ClampMin = "0", ClampMax = "9"))
    int32 HitGroup{0};
};