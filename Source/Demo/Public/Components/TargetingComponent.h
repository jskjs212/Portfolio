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
 * Search for potential targets within a distance, then lock onto
 *     1) the closest target within a cone from the character to the camera's forward position,
 *     2) the closest target within a sphere from the character (including backwards).
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
    UTargetingComponent();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Targeting functions
    ////////////////////////////////////////////////////////
public:
    void LockTarget();

    void UnlockTarget();

    void ToggleTargetLock();

private:
    FFindTargetResult FindTarget();

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
    float BackTargetDistance{500.f};

    UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType{};

    //UPROPERTY(EditAnywhere, Category = "Initialization|Targeting")
    //FName TargetSocketName{TEXT("")};

private:
    static constexpr float CameraRotationInterpSpeed{7.f};

    static inline const TArray<TEnumAsByte<EObjectTypeQuery>> TargetObjectTypes{UEngineTypes::ConvertToObjectType(ECC_Pawn)};

    static inline const TArray<AActor*> ActorsToIgnore{};
};