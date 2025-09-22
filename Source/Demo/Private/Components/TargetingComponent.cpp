// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TargetingComponent.h"
#include "Interfaces/TargetInterface.h"
#include "Kismet/KismetSystemLibrary.h"

UTargetingComponent::UTargetingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTargetingComponent::BeginPlay()
{
    Super::BeginPlay();

    // Trace settings
    //ActorsToIgnore.Add(GetOwner());
}

void UTargetingComponent::LockTarget()
{
    FFindTargetResult FindResult = FindTarget();
    if (!FindResult.bFound)
    {
        return;
    }

    // @TODO - Can be targeted?
    FindResult.TargetInterface->OnTargeted(true);

    SetTargetStatus(true, FindResult.Target);
}

void UTargetingComponent::UnlockTarget()
{
    if (IsValid(LockedTarget))
    {
        if (ITargetInterface* TargetInterface = Cast<ITargetInterface>(LockedTarget))
        {
            TargetInterface->OnTargeted(false);
        }
    }

    SetTargetStatus(false, nullptr);
    OnTargetUnlocked.ExecuteIfBound();
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

FFindTargetResult UTargetingComponent::FindTarget()
{
    constexpr float DebugDrawTime = 3.f;
    FVector Center{};
    TArray<FHitResult> HitResults;

    // Potential targets
    // Trace all because we are not planning to spawn many enemies like Hack & Slash games.
    // @check - What if there are thousands of pawns?
    bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        this, /* WorldContextObject */
        Center,
        Center,
        MaxTargetDistance,
        TargetObjectTypes,
        false, /* bTraceComplex */
        ActorsToIgnore,
        DrawDebugType,
        HitResults,
        true, /* bIgnoreSelf */
        FLinearColor::Red,
        FLinearColor::Green,
        DebugDrawTime
    );
    if (!bHit)
    {
        return {};
    }

    // @TODO - Calculate angle and distance
    UE_LOG(LogTemp, Warning, TEXT("HitResults.Num(): %d"), HitResults.Num());
    for (const FHitResult& Hit : HitResults)
    {
        if (ITargetInterface* HitTargetInterface = Cast<ITargetInterface>(Hit.GetActor()))
        {
            // @TEST
            return FFindTargetResult{true, Hit.GetActor(), HitTargetInterface};
        }
    }

    // @TODO - Find the closest target within a cone from the character to the camera's forward position.

    // @TODO - Find the closest target within a sphere from the character (including backwards).

    return {};
}