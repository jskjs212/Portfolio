// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DemoAnimInstance.generated.h"

class ABaseCharacter;

/**
 * An animation state proxy used for thread-safe updates.
 * Data is stored on the game thread and accessed (read-only) in ThreadSafeUpdate().
 */
struct FDemoAnimState
{
    FVector Velocity{FVector::ZeroVector};

    float PrevAimOffsetYaw{0.f};

    FRotator ActorRotation{FRotator::ZeroRotator};

    FRotator AimRotation{FRotator::ZeroRotator};

    FRotator PrevStrafeRotation{FRotator::ZeroRotator};
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
    virtual void NativeBeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Update functions
    ////////////////////////////////////////////////////////
protected:
    void UpdateCharacterStateOnGameThread();

    /* Thread-safe update functions */
    // Read-only access to AnimState.
    // Write-only access to member variables.
    // No other access to game thread data.
    void UpdateSpeed();
    void UpdateAimOffset(float DeltaSeconds);
    void UpdateStrafeOffset(float DeltaSeconds);

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    void HandleBlockingStateChanged(bool bNewBlocking) { bIsBlocking = bNewBlocking; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    bool bIsInAir{false};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    bool bIsAscending{false};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    bool bIsAccelerating{false};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    bool bIsBlocking{false};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    float GroundSpeed{0.f};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    float AimOffsetYaw{0.f};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    float AimOffsetPitch{0.f};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    float StrafeOffsetYaw{0.f};

    UPROPERTY(BlueprintReadOnly, Transient, Category = "Character")
    TObjectPtr<ABaseCharacter> BaseCharacter;

private:
    static constexpr float AimOffsetInterpSpeed{6.f};
    static constexpr float StrafeOffsetInterpSpeed{6.f};

    FRotator StrafeRotation;

    FDemoAnimState AnimState;
};