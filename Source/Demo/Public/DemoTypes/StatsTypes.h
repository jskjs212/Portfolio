// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatsTypes.generated.h"

/**
 * Stat format for Health, Mana, Stamina, etc.
 */
USTRUCT()
struct DEMO_API FResourceStat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Stats")
    float CurrentValue{0.f};

    UPROPERTY(EditAnywhere, Category = "Stats")
    float MaxValue{100.f};

    UPROPERTY(EditAnywhere, Category = "Stats")
    bool bCanRegen{false};

    // Per second!!!
    UPROPERTY(EditAnywhere, Category = "Stats")
    float RegenRate{5.f};

    // Seconds
    UPROPERTY(EditAnywhere, Category = "Stats")
    float RegenInterval{0.1f};

    // Seconds
    UPROPERTY(EditAnywhere, Category = "Stats")
    float RegenDelay{1.f};

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
};

/**
 * Stat format for STR, DEX, INT, etc.
 */
 //USTRUCT(BlueprintType)
 //struct DEMO_API FPrimaryStat
 //{
 //    GENERATED_BODY()
 //
 //    UPROPERTY(EditAnywhere, Category = "Stats")
 //    float BaseValue{0.f};
 //
 //    UPROPERTY(EditAnywhere, Category = "Stats")
 //    float AssignedValue{0.f};
 //};

 /**
  * Stat format for Attack, Defense, Speed, etc.
  */
  //USTRUCT(BlueprintType)
  //struct DEMO_API FDerivedStat
  //{
  //    GENERATED_BODY()
  //
  //    UPROPERTY(EditAnywhere, Category = "Stats")
  //    float BaseValue{0.f};
  //};