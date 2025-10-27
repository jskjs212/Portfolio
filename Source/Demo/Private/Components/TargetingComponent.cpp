// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TargetingComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/TargetInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Static variable initialization
const TArray<TEnumAsByte<EObjectTypeQuery>> UTargetingComponent::TargetObjectTypes{UEngineTypes::ConvertToObjectType(ECC_Pawn)};

UTargetingComponent::UTargetingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    VisibilityQueryParams.AddIgnoredActor(GetOwner());
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

#if WITH_EDITORONLY_DATA
            if (bDrawDebugInfo)
            {
                // Draw a sphere at the target
                UKismetSystemLibrary::DrawDebugSphere(this, FindResult.Target->GetActorLocation(), 50.f, 12, FLinearColor::Red, DrawDebugDuration);
            }
#endif // WITH_EDITORONLY_DATA
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

TArray<FHitResult> UTargetingComponent::GetSweepResults(const AActor* OwnerActor, FVector StartLocation, FVector DirectionNormal) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return {};
    }

    // Find all pawns within the sphere
    TArray<FHitResult> HitResults;
    UKismetSystemLibrary::SphereTraceMultiForObjects(
        this, /* WorldContextObject */
        StartLocation,
        StartLocation,
        MaxTargetDistance,
        TargetObjectTypes,
        false, /* bTraceComplex */
        EmptyIgnoredActors,
        EDrawDebugTrace::None,
        HitResults,
        true /* bIgnoreSelf */
    );

#if WITH_EDITORONLY_DATA
    if (bDrawDebugInfo)
    {
        // Draw spheres and some edges of cones
        const FVector DirectionTotal = DirectionNormal * MaxTargetDistance;
        UKismetSystemLibrary::DrawDebugSphere(this, StartLocation, MaxTargetDistance, 12, FLinearColor::Green, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugSphere(this, StartLocation, OmnidirectionalTargetDistance, 12, FLinearColor::Yellow, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(FirstTargetingConeAngle, FVector::UpVector), FLinearColor::Yellow, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(-FirstTargetingConeAngle, FVector::UpVector), FLinearColor::Yellow, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(SecondTargetingConeAngle, FVector::UpVector), FLinearColor::Yellow, DrawDebugDuration);
        UKismetSystemLibrary::DrawDebugLine(this, StartLocation, StartLocation + DirectionTotal.RotateAngleAxis(-SecondTargetingConeAngle, FVector::UpVector), FLinearColor::Yellow, DrawDebugDuration);
    }
#endif // WITH_EDITORONLY_DATA

    return HitResults;
}

bool UTargetingComponent::IsImpactPointVisible(const UWorld* World, const AActor* Target, FVector StartLocation, FVector ImpactPoint) const
{
    FHitResult HitResult;
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        ImpactPoint,
        ECC_Visibility,
        VisibilityQueryParams
    );
    return !bHit; // Nothing between the camera and the target
}

FFindTargetResult UTargetingComponent::FindTargetFromSweepResults(const TArray<FHitResult>& HitResults, FVector StartLocation, FVector DirectionNormal, FVector CameraLocation) const
{
    const float FirstDotThreshold = FMath::Cos(FMath::DegreesToRadians(FirstTargetingConeAngle));
    const float SecondDotThreshold = FMath::Cos(FMath::DegreesToRadians(SecondTargetingConeAngle));

    float ClosestDistanceSqrInFirstCone = UE_MAX_FLT;
    AActor* ClosestTargetInFirstCone = nullptr;

    float ClosestDistanceSqrInSecondCone = UE_MAX_FLT;
    AActor* ClosestTargetInSecondCone = nullptr;

    float ClosestDistanceSqr = FMath::Square(OmnidirectionalTargetDistance) + UE_SMALL_NUMBER; // Inside the distance
    AActor* ClosestTarget = nullptr;

    // Repeat multiple times for the same target, for the case that camera focuses out of center of the target (e.g. hands, feet).
    const UWorld* World = GetWorld();
    for (const FHitResult& Hit : HitResults)
    {
        AActor* Target = Hit.GetActor();
        if (!IsValid(Target) || !Cast<ITargetInterface>(Target))
        {
            continue;
        }

        const FVector ToImpactPoint = Hit.ImpactPoint - StartLocation;
        const float DistanceSqr = ToImpactPoint.SizeSquared();
        const float DotProduct = FVector::DotProduct(DirectionNormal, ToImpactPoint.GetSafeNormal());

        // Find the closest target within the FirstTargetingConeAngle
        if (DistanceSqr < ClosestDistanceSqrInFirstCone && DotProduct >= FirstDotThreshold)
        {
            // The number of line traces inside this block is 0.1~2% of total hit results.
            if (IsImpactPointVisible(World, Target, CameraLocation, Hit.ImpactPoint))
            {
                ClosestDistanceSqrInFirstCone = DistanceSqr;
                ClosestTargetInFirstCone = Target;
                continue;
            }
        }

        // Find the closest target within the SecondTargetingConeAngle
        if (DistanceSqr < ClosestDistanceSqrInSecondCone && DotProduct >= SecondDotThreshold)
        {
            if (IsImpactPointVisible(World, Target, CameraLocation, Hit.ImpactPoint))
            {
                ClosestDistanceSqrInSecondCone = DistanceSqr;
                ClosestTargetInSecondCone = Target;
                continue;
            }
        }

        // Find the closest target within OmnidirectionalTargetDistance
        if (DistanceSqr < ClosestDistanceSqr)
        {
            if (IsImpactPointVisible(World, Target, CameraLocation, Hit.ImpactPoint))
            {
                ClosestDistanceSqr = DistanceSqr;
                ClosestTarget = Target;
            }
        }
    }

    if (ClosestTargetInFirstCone)
    {
        return {true, ClosestTargetInFirstCone, Cast<ITargetInterface>(ClosestTargetInFirstCone)};
    }

    if (ClosestTargetInSecondCone)
    {
        return {true, ClosestTargetInSecondCone, Cast<ITargetInterface>(ClosestTargetInSecondCone)};
    }

    if (ClosestTarget)
    {
        return {true, ClosestTarget, Cast<ITargetInterface>(ClosestTarget)};
    }

    return {}; // Not found
}

FFindTargetResult UTargetingComponent::FindTarget() const
{
    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UTargetingComponent::FindTarget"), STAT_TargetingComponent_FindTarget, STATGROUP_Game);
    TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT(__FUNCTION__));

    // Assumes that the owner has only one camera component.
    const AActor* OwnerActor = GetOwner();
    const UCameraComponent* Camera = OwnerActor ? OwnerActor->FindComponentByClass<UCameraComponent>() : nullptr;
    if (!Camera)
    {
        return {};
    }

    // Calculate location and direction
    constexpr float SpringArmLengthEstimate = 300.f; // Maybe vary in-game, but not that important
    const FVector StartLocation = OwnerActor->GetActorLocation();
    const FVector CameraLocation = Camera->GetComponentLocation();
    const FVector EndLocationEstimate = CameraLocation + (Camera->GetForwardVector() * (MaxTargetDistance + SpringArmLengthEstimate)); // Rough estimate of end location
    const FVector DirectionNormal = (EndLocationEstimate - StartLocation).GetUnsafeNormal();

    // Get pawns inside the sphere
    const TArray<FHitResult> HitResults = GetSweepResults(OwnerActor, StartLocation, DirectionNormal);
    if (!HitResults.IsEmpty())
    {
        return FindTargetFromSweepResults(HitResults, StartLocation, DirectionNormal, CameraLocation);
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
    FVector TargetLocation = Cast<ITargetInterface>(LockedTarget)->GetTargetPointLocation();
    TargetLocation.Z += TargetingPointHeightAdjustment; // Look lower than the target location

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