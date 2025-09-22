// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/DemoAnimInstance.h"
#include "Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UDemoAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    BaseCharacter = Cast<ABaseCharacter>(GetOwningActor());

#if WITH_EDITOR
    // Use default object for editor preview
    if (const UWorld* World = GetWorld())
    {
        if (!BaseCharacter && !World->IsGameWorld())
        {
            BaseCharacter = GetMutableDefault<ABaseCharacter>();
        }
    }
#endif // WITH_EDITOR

    if (!BaseCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("UDemoAnimInstance::NativeInitializeAnimation - BaseCharacter is nullptr."));
    }
}

void UDemoAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    UpdateCharacterStateOnGameThread();
}

void UDemoAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    // This function is called on the game thread in editor preview (or immediate update?).
    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UDemoAnimInstance::NativeThreadSafeUpdateAnimation"), STAT_UDemoAnimInstance_NativeThreadSafeUpdateAnimation, STATGROUP_Anim);
    TRACE_CPUPROFILER_EVENT_SCOPE_STR(__FUNCTION__);

    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

    UpdateSpeed();
    UpdateAimOffset(DeltaSeconds);
    UpdateStrafeOffset(DeltaSeconds);
}

void UDemoAnimInstance::UpdateCharacterStateOnGameThread()
{
    check(IsInGameThread());

    if (!IsValid(BaseCharacter))
    {
        return;
    }

    bIsInAir = BaseCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().SizeSquared() > UE_KINDA_SMALL_NUMBER;

    AnimState.Velocity = BaseCharacter->GetVelocity();
    AnimState.PrevAimOffsetYaw = AimOffsetYaw;
    AnimState.ActorRotation = BaseCharacter->GetActorRotation();
    AnimState.AimRotation = BaseCharacter->GetBaseAimRotation();
    AnimState.PrevStrafeRotation = StrafeRotation;
}

void UDemoAnimInstance::UpdateSpeed()
{
    FVector Velocity = AnimState.Velocity;
    Velocity.Z = 0.f;
    GroundSpeed = Velocity.Size();
}

void UDemoAnimInstance::UpdateAimOffset(float DeltaSeconds)
{
    constexpr float AimOffsetYawLimit = 135.f;
    const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AnimState.AimRotation, AnimState.ActorRotation);

    AimOffsetYaw = UKismetMathLibrary::FInterpTo(AnimState.PrevAimOffsetYaw, DeltaRotation.Yaw, DeltaSeconds, AimOffsetInterpSpeed);
    if (FMath::Abs(AimOffsetYaw) > AimOffsetYawLimit)
    {
        // Look front
        AimOffsetYaw = 0.f;
    }

    AimOffsetPitch = DeltaRotation.Pitch;
}

void UDemoAnimInstance::UpdateStrafeOffset(float DeltaSeconds)
{
    const FRotator MovementRotation = AnimState.Velocity.ToOrientationRotator();
    const FRotator TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AnimState.ActorRotation);
    const FRotator NewStrafeRotation = UKismetMathLibrary::RInterpTo(AnimState.PrevStrafeRotation, TargetRotation, DeltaSeconds, StrafeOffsetInterpSpeed);

    StrafeRotation = NewStrafeRotation;
    StrafeOffsetYaw = NewStrafeRotation.Yaw;
}