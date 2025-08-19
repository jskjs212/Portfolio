// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStatsComponent();

protected:
    virtual void BeginPlay() override;
};