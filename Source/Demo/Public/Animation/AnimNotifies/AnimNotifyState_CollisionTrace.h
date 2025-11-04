// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "DemoTypes/AttackTypes.h"
#include "AnimNotifyState_CollisionTrace.generated.h"

/**
 * Activate attack collision of a certain type (definition) during the notify duration.
 * Same type and same hit group at the same time will be deactivated at the end of the first notify of that type.
 */
UCLASS(meta = (DisplayName = "Collision Trace"))
class DEMO_API UAnimNotifyState_CollisionTrace : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    UAnimNotifyState_CollisionTrace(const FObjectInitializer& ObjectInitializer);

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    // The type of attack collision definition to activate.
    UPROPERTY(EditAnywhere, Category = "Initialization")
    EAttackCollisionType AttackCollisionType{EAttackCollisionType::None};

    // @WARNING This must be true for first notify of the same type in a montage, because hit actors are not cleared on end.
    // true: Allows multiple hits on the same actor in one attack sequence.
    // false: Allows one unique hit when attacking multiple time.
    //        Even if the Type is different, already hit actor won't be hit again if the HitGroup is the same.
    UPROPERTY(EditAnywhere, Category = "Initialization")
    bool bClearHitActorsOnBegin{true};

    // @WARNING The array of TSet will grow as needed, and persist for the lifetime of the component.
    // Differentiates hit groups (TSet<AActor*> HitActors) in the AttackCollisionComponent.
    // Same group: Allows one unique hit for different types (or for different notifies if bClearHitActorsOnBegin is false).
    // Different group: Allows multiple hits on the same actor, even if the type is the same.
    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (ClampMin = "0", ClampMax = "9"))
    int32 HitGroup{0};
};