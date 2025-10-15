// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DemoTypes/StatsTypes.h"
#include "GameplayTagContainer.h"
#include "DemoPawnData.generated.h"

class UDemoInputConfig;

/**
 *
 */
UCLASS(Const)
class DEMO_API UDemoPawnData : public UDataAsset
{
    GENERATED_BODY()

public:
    // Identity of the character, used for animation selection.
    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Character"))
    FGameplayTag CharacterTag;

    // Default Health, Stamina, etc.
    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Stat"))
    TMap<FGameplayTag, FResourceStat> DefaultResourceStats;

    // Input configuration for PlayerCharacter.
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UDemoInputConfig> InputConfig;
};