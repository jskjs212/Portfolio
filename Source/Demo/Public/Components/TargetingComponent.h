// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

class ITargetInterface;

struct FFindTargetResult
{
    bool bFound{false};

    AActor* Target{nullptr};

    ITargetInterface* TargetInterface{nullptr};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetUpdated, AActor* /* NewTarget */);

/**
 * Handles targeting (Lock-On) functionality for a character.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UTargetingComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnTargetUpdated OnTargetUpdated;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UTargetingComponent();

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    ////////////////////////////////////////////////////////
    //        Targeting functions
    ////////////////////////////////////////////////////////
public:
    void LockTarget();

    void UnlockTarget();

    void ToggleTargetLock();

private:
    // Get all sweep results that hit pawns
    TArray<FHitResult> GetSweepResults(const AActor* OwnerActor, FVector StartLocation, FVector DirectionNormal) const;

    bool IsImpactPointVisible(const UWorld* World, const AActor* Target, FVector StartLocation, FVector ImpactPoint) const;

    // Find a target from the sweep results.
    FFindTargetResult FindTargetFromSweepResults(const TArray<FHitResult>& HitResults, FVector StartLocation, FVector DirectionNormal, FVector CameraLocation) const;

    // 1) Search for pawns within the MaxTargetDistance.
    // 2) Find the closest target within the FirstTargetingConeAngle from the character to the camera's forward position.
    // 3) Find the closest target within the SecondTargetingConeAngle from the character to the camera's forward position.
    // 4) If not found, find the closest target within the OmnidirectionalTargetDistance from the character (including backwards).
    // Insights: 1~2ms for 100 pawns
    // To optimize: Decrease the distance, or use AsyncSweep?
    // @return If bFound is true, Target and TargetInterface are valid.
    FFindTargetResult FindTarget() const;

    bool IsTargetStillValid() const;

    // Rotate the camera to face the locked target.
    // Unlock if the target is no longer valid.
    // @check - Called every tick.
    void UpdateTargetingCamera(float DeltaTime);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE bool IsTargetLocked() const { return bIsTargetLocked; }

private:
    void SetTargetStatus(bool bLocked, AActor* NewTarget = nullptr);

    ////////////////////////////////////////////////////////
    //        Variables - target status
    ////////////////////////////////////////////////////////
private:
    bool bIsTargetLocked{false};

    TObjectPtr<AActor> LockedTarget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    // Maximum distance to search
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Targeting")
    float MaxTargetDistance{1500.f};

    // [degree] The half angle of the cone (from center to edge) in front of the character
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Targeting", meta = (ClampMin = "0.", ClampMax = "180."))
    float FirstTargetingConeAngle{8.f};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Targeting", meta = (ClampMin = "0.", ClampMax = "180."))
    float SecondTargetingConeAngle{18.f};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Targeting")
    float OmnidirectionalTargetDistance{500.f}; // 360 degree search

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Debug")
    bool bDrawDebugInfo{false};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Debug")
    float DrawDebugDuration{3.f};
#endif // WITH_EDITORONLY_DATA

private:
    static constexpr float TargetingPointHeightAdjustment{-100.f};

    static constexpr float CameraRotationInterpSpeed{5.f};

    static inline const TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectTypes{UEngineTypes::ConvertToObjectType(ECC_Pawn)};

    static inline const TArray<AActor*> EmptyIgnoredActors;

    FCollisionQueryParams VisibilityQueryParams;
};