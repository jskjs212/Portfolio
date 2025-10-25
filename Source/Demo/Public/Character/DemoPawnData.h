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
    // Consider refactoring to PawnTag for vehicles?
    UPROPERTY(EditDefaultsOnly, Category = "Character", meta = (Categories = "Character"))
    FGameplayTag CharacterTag;

    // Name displayed in UI.
    UPROPERTY(EditDefaultsOnly, Category = "Character")
    FText CharacterDisplayName;

    // Boss has special behaviors and UI.
    UPROPERTY(EditDefaultsOnly, Category = "Character")
    bool bIsBoss{false};

    // Default Health, Stamina, etc.
    UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (Categories = "Stat"))
    TMap<FGameplayTag, FResourceStat> DefaultResourceStats;

    // Default STR, DEX, INT, etc.
    UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (Categories = "Stat"))
    TMap<FGameplayTag, FPrimaryStat> DefaultPrimaryStats;

    // Default Attack, Defense, etc.
    UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (Categories = "Stat"))
    TMap<FGameplayTag, FDerivedStat> DefaultDerivedStats;

    // Input configuration for PlayerCharacter.
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UDemoInputConfig> InputConfig;
};