// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DemoAnimInstance.generated.h"

class ABaseCharacter;

/**
 * An animation state proxy used for thread-safe updates.
 * Data is stored on the game thread and accessed in ThreadSafeUpdate().
 */
struct FDemoAnimState
{
    //bool bIsInAir = false;
    //float DeltaSeconds = 0.f;

    FVector Velocity{FVector::ZeroVector};
    //FVector Acceleration = FVector::ZeroVector;
};

/**
 *
 */
UCLASS()
class DEMO_API UDemoAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

    ////////////////////////////////////////////////////////
    //        Update functions
    ////////////////////////////////////////////////////////
    void UpdateCharacterStateOnGameThread();

    void UpdateSpeed();

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    FDemoAnimState AnimState;

    UPROPERTY(BlueprintReadOnly, Category = "Character", Transient)
    bool bIsInAir{false};

    UPROPERTY(BlueprintReadOnly, Category = "Character", Transient)
    bool bIsAccelerating{false};

    UPROPERTY(BlueprintReadOnly, Category = "Character", Transient)
    float GroundSpeed{0.f};

    UPROPERTY(BlueprintReadOnly, Category = "Character", Transient)
    TObjectPtr<ABaseCharacter> BaseCharacter;
};