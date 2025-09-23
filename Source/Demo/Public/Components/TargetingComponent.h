// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

namespace EDrawDebugTrace { enum Type : int; }
class ITargetInterface;

struct FFindTargetResult
{
    bool bFound{false};

    AActor* Target{nullptr};

    ITargetInterface* TargetInterface{nullptr};
};

/**
 * Handles targeting (Lock-On) functionality for a character.
 * Search for pawns within the MaxTargetDistance, then lock onto
 *     1) the closest target within the TargetingConeAngle from the character to the camera's forward position,
 *     2) the closest target within the OmnidirectionalTargetDistance from the character (including backwards).
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UTargetingComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FSimpleDelegate OnTargetUnlocked;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UTargetingComponent();

protected:
    virtual void BeginPlay() override;

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
    // Get overlapping pawns that implement the TargetInterface
    TSet<AActor*> GetOverlappingTargets(const AActor* OwnerActor, FVector StartLocation, FVector DirectionNormal) const;

    // 1) Search for pawns within the MaxTargetDistance.
    // 2) Find the closest target within the TargetingConeAngle from the character to the camera's forward position.
    // 3) If not found, find the closest target within the OmnidirectionalTargetDistance from the character (including backwards).
    // 1.5~5.5ms for 100 pawns
    // To optimize: Decrease the distance, or use AsyncOverlap?
    // @return If bFound is true, Target and TargetInterface are valid.
    FFindTargetResult FindTarget();

    bool IsTargetStillValid() const;

    // Rotate the camera to face the locked target.
    // Unlock if the target is no longer valid.
    void UpdateTargetingCamera(float DeltaTime);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE bool IsTargetLocked() const { return bIsTargetLocked; }

private:
    void SetTargetStatus(bool bLocked, AActor* NewTarget = nullptr)
    {
        bIsTargetLocked = bLocked;
        LockedTarget = NewTarget;
    }

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
    UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    float MaxTargetDistance{1500.f};

    // [degree] The half angle of the cone (from center to edge) in front of the character
    UPROPERTY(EditAnywhere, Category = "Initialization|Targeting", meta = (ClampMin = "0.", ClampMax = "180."))
    float TargetingConeAngle{15.f};

    UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    float OmnidirectionalTargetDistance{500.f}; // 360 degree search

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    bool bDrawDebugInfo{false};

    UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    float DrawDebugDuration{3.f};
#endif // WITH_EDITORONLY_DATA

    //UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    //TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType{};

    //UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    //FName TargetSocketName{TEXT("")};

private:
    static constexpr float TargetingPointHeightAdjustment{-100.f};

    static constexpr float CameraRotationInterpSpeed{7.f};

    static inline const FCollisionObjectQueryParams TargetObjectQueryParams{ECC_Pawn};

    //static inline const TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectTypes{UEngineTypes::ConvertToObjectType(ECC_Pawn)};

    //static inline const TArray<AActor*> ActorsToIgnore{};
};