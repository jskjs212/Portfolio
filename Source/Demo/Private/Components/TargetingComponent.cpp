// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TargetingComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/TargetInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h" // @TEST
#include "Components/SkeletalMeshComponent.h" // @TEST


UTargetingComponent::UTargetingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsTargetLocked)
    {
        UpdateTargetingCamera(DeltaTime);
    }
}

void UTargetingComponent::LockTarget()
{
    FFindTargetResult FindResult = FindTarget();
    if (FindResult.bFound)
    {
        if (FindResult.TargetInterface->CanBeTargeted())
        {
            FindResult.TargetInterface->OnTargeted(true);
            SetTargetStatus(true, FindResult.Target);

#if WITH_EDITOR
            if (bDrawDebugInfo)
            {
                UKismetSystemLibrary::DrawDebugSphere(this, FindResult.Target->GetActorLocation(), 50.f, 12, FLinearColor::Red, DrawDebugDuration);
            }
#endif // WITH_EDITOR
        }
    }
}

void UTargetingComponent::UnlockTarget()
{
    if (IsValid(LockedTarget))
    {
        ITargetInterface* TargetInterface = Cast<ITargetInterface>(LockedTarget);
        TargetInterface->OnTargeted(false); // Checked in FindTarget()
    }

    SetTargetStatus(false, nullptr);
}

void UTargetingComponent::ToggleTargetLock()
{
    if (bIsTargetLocked)
    {
        UnlockTarget();
    }
    else
    {
        LockTarget();
    }
}

TSet<AActor*> UTargetingComponent::GetOverlappingTargets(const AActor* OwnerActor, FVector StartLocation, FVector DirectionNormal) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return {};
    }

    // Overlap parameters
    const FCollisionShape Sphere = FCollisionShape::MakeSphere(MaxTargetDistance);
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerActor);

    // Find all pawns within the sphere
    bool bHit = World->OverlapMultiByObjectType(
        OverlapResults,
        StartLocation,
        FQuat::Identity,
        TargetObjectQueryParams,
        Sphere,
        QueryParams
    );

#if WITH_EDITOR
    if (bDrawDebugInfo)
    {
        // Draw spheres and some edges of cones
        const FVector DirectionTotal = DirectionNormal * MaxTargetDistance;
        UKismetSystemLibrary::DrawDebugSphere(this, StartLocation, MaxTargetDistance, 12, FLinearColor::Green, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugSphere(this, StartLocation, OmnidirectionalTargetDistance, 12, FLinearColor::Green, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(FirstTargetingConeAngle, FVector::UpVector), FLinearColor::Gray, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(-FirstTargetingConeAngle, FVector::UpVector), FLinearColor::Gray, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(SecondTargetingConeAngle, FVector::UpVector), FLinearColor::Gray, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(-SecondTargetingConeAngle, FVector::UpVector), FLinearColor::Gray, DrawDebugDuration);
    }
#endif // WITH_EDITOR

    if (!bHit)
    {
        return {};
    }

    // Filter valid targets
    TSet<AActor*> UniqueTargets;
    for (const FOverlapResult& Overlap : OverlapResults)
    {
        AActor* OverlappedActor = Overlap.GetActor();
        if (IsValid(OverlappedActor))
        {
            if (ITargetInterface* TargetInterface = Cast<ITargetInterface>(OverlappedActor))
            {
                UniqueTargets.Add(OverlappedActor);
            }
        }
    }

#if WITH_EDITOR
    if (bDrawDebugInfo)
    {
        // Draw overlapped targets
        for (AActor* Target : UniqueTargets)
        {
            UKismetSystemLibrary::DrawDebugSphere(this, Target->GetActorLocation(), 30.f, 12, FLinearColor::Blue, DrawDebugDuration);
        }
    }
#endif // WITH_EDITOR

    return UniqueTargets;
}

FFindTargetResult UTargetingComponent::FindTarget()
{
    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UTargetingComponent::FindTarget"), STAT_TargetingComponent_FindTarget, STATGROUP_Game);
    TRACE_CPUPROFILER_EVENT_SCOPE_STR(__FUNCTION__);

    // Assumes that the owner has only one camera component.
    const AActor* OwnerActor = GetOwner();
    const UCameraComponent* Camera = OwnerActor ? OwnerActor->FindComponentByClass<UCameraComponent>() : nullptr;
    if (!Camera)
    {
        return {};
    }

    // Calculate location and direction
    constexpr float SpringArmLengthEstimate = 300.f; // Maybe variant in-game, but not that important.
    const FVector StartLocation = OwnerActor->GetActorLocation();
    const FVector EndLocationEstimate = Camera->GetComponentLocation() + (Camera->GetForwardVector() * (MaxTargetDistance + SpringArmLengthEstimate));
    const FVector DirectionNormal = (EndLocationEstimate - StartLocation).GetUnsafeNormal();

    // Get overlapping targets
    //const TSet<AActor*> Targets = GetOverlappingTargets(OwnerActor, StartLocation, DirectionNormal);
    TSet<AActor*> Targets = GetOverlappingTargets(OwnerActor, StartLocation, DirectionNormal);
    if (Targets.IsEmpty())
    {
        return {};
    }

    float ClosestDistanceSqrInFirstCone = UE_MAX_FLT;
    AActor* ClosestTargetInFirstCone = nullptr;

    float ClosestDistanceSqr = FMath::Square(OmnidirectionalTargetDistance) + UE_SMALL_NUMBER; // Inside the distance
    AActor* ClosestTarget = nullptr;

    // Calculate angle and distance
    const float DotThreshold = FMath::Cos(FMath::DegreesToRadians(FirstTargetingConeAngle));
    for (AActor* Target : Targets)
    {
        const FVector ToTarget = Target->GetActorLocation() - StartLocation;
        const float DistanceSqr = ToTarget.SizeSquared();
        const float DotProduct = FVector::DotProduct(DirectionNormal, ToTarget.GetSafeNormal());

        // Find the closest target within the FirstTargetingConeAngle
        if (DistanceSqr < ClosestDistanceSqrInFirstCone && DotProduct >= DotThreshold)
        {
            ClosestDistanceSqrInFirstCone = DistanceSqr;
            ClosestTargetInFirstCone = Target;
        }

        // Find the closest target within OmnidirectionalTargetDistance
        if (DistanceSqr < ClosestDistanceSqr)
        {
            ClosestDistanceSqr = DistanceSqr;
            ClosestTarget = Target;
        }
    }

    if (ClosestTargetInFirstCone)
    {
        return {true, ClosestTargetInFirstCone, Cast<ITargetInterface>(ClosestTargetInFirstCone)};
    }

    if (ClosestTarget)
    {
        return {true, ClosestTarget, Cast<ITargetInterface>(ClosestTarget)};
    }

    return {}; // Not found
}

bool UTargetingComponent::IsTargetStillValid() const
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor || !IsValid(LockedTarget))
    {
        return false;
    }

    ITargetInterface* TargetInterface = Cast<ITargetInterface>(LockedTarget);
    if (!TargetInterface->CanBeTargeted())
    {
        return false;
    }

    const float DistanceSqr = (LockedTarget->GetActorLocation() - OwnerActor->GetActorLocation()).SizeSquared();
    if (DistanceSqr > FMath::Square(MaxTargetDistance))
    {
        return false;
    }

    return true;
}

void UTargetingComponent::UpdateTargetingCamera(float DeltaTime)
{
    APawn* OwnerPawn = GetOwner<APawn>();
    AController* OwnerController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
    if (!OwnerController || !IsTargetStillValid())
    {
        // @check - Find next target automatically?
        UnlockTarget();
        return;
    }

    const FVector OwnerLocation = OwnerPawn->GetActorLocation();
    FVector TargetLocation = LockedTarget->GetActorLocation();
    TargetLocation.Z += TargetingPointHeightAdjustment; // Look lower than the center

    const FRotator CurrentRotation = OwnerController->GetControlRotation();
    const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);

    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, CameraRotationInterpSpeed);
    NewRotation.Roll = CurrentRotation.Roll;

    OwnerController->SetControlRotation(NewRotation);
}

void UTargetingComponent::SetTargetStatus(bool bLocked, AActor* NewTarget)
{
    bIsTargetLocked = bLocked;
    LockedTarget = NewTarget;

    OnTargetUpdated.Broadcast(NewTarget);
}