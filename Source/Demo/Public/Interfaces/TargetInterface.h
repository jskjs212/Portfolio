// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTargetInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class DEMO_API ITargetInterface
{
    GENERATED_BODY()

public:
    virtual void OnTargeted(bool bIsTargeted) PURE_VIRTUAL(ITargetInterface::OnTargeted);

    virtual bool CanBeTargeted() const PURE_VIRTUAL(ITargetInterface::CanBeTargeted, return false;);

    virtual FVector GetTargetPointLocation() const PURE_VIRTUAL(ITargetInterface::GetTargetPointLocation, return FVector::ZeroVector;);
};