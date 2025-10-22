// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StatsTypes.generated.h"

/**
 * Stat format for Health, Mana, Stamina, etc.
 * @TODO - Add StatModifiers -> Add parent struct for all stats.
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

UENUM()
enum class EStatModOp : uint8
{
    Add UMETA(DisplayName = "Additive"),
    Multiply UMETA(DisplayName = "Multiplicative")
};

/**
 * Modifier format for buffs, debuffs, equipment, etc.
 * @TODO Duration, Handle(GUID)
 */
USTRUCT()
struct FStatModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Stats")
    EStatModOp Operation{EStatModOp::Add};

    // If Multiplicative, 0.1 means (Value) * (1 + 0.1 + (sum of other modifiers))
    UPROPERTY(EditAnywhere, Category = "Stats")
    float Magnitude{0.f};

    UPROPERTY(EditAnywhere, Category = "Stats")
    FGameplayTag SourceTag;

    // @check - == ignores Magnitude
    bool operator==(const FStatModifier& Other) const
    {
        return Operation == Other.Operation && SourceTag == Other.SourceTag;
    }
};

namespace StatModOpHelper
{
inline float Aggregate(float BaseLikeValue, const TArray<FStatModifier>& Modifiers)
{
    float ToAdd = 0.f;
    float ToMultiply = 1.f;

    for (const FStatModifier& Modifier : Modifiers)
    {
        switch (Modifier.Operation)
        {
        case EStatModOp::Add:
            ToAdd += Modifier.Magnitude;
            break;
        case EStatModOp::Multiply:
            ToMultiply += Modifier.Magnitude;
            break;
        default:
            break;
        }
    }

    return (BaseLikeValue + ToAdd) * ToMultiply;
}
} // namespace StatModOpHelper

/**
 * Stat format for STR, DEX, INT, etc.
 * @check - Cache final value?
 */
USTRUCT()
struct DEMO_API FPrimaryStat
{
    GENERATED_BODY()

    // Base value from level.
    UPROPERTY(EditAnywhere, Category = "Stats")
    float BaseValue{0.f};

    // Points assigned by player.
    UPROPERTY(EditAnywhere, Category = "Stats")
    float AssignedValue{0.f};

    UPROPERTY(EditAnywhere, Category = "Stats")
    TArray<FStatModifier> Modifiers;

    bool AddModifier(const FStatModifier& Modifier)
    {
        if (Modifiers.Contains(Modifier))
        {
            return false;
        }
        Modifiers.Add(Modifier);
        return true;
    }

    bool RemoveModifier(const FStatModifier& Modifier)
    {
        return Modifiers.RemoveSingle(Modifier) > 0;
    }

    float GetFinalValue() const
    {
        const float BaseLike = BaseValue + AssignedValue;
        return StatModOpHelper::Aggregate(BaseLike, Modifiers);
    }
};

/**
 * Stat format for Attack, Defense, Speed, etc.
 */
USTRUCT()
struct DEMO_API FDerivedStat
{
    GENERATED_BODY()

    // Base value computed from formula.
    // e.g. Attack = STR + WeaponDamage
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    float BaseValue{0.f};

    UPROPERTY(EditAnywhere, Category = "Stats")
    TArray<FStatModifier> Modifiers;

    bool AddModifier(const FStatModifier& Modifier)
    {
        if (Modifiers.Contains(Modifier))
        {
            return false;
        }
        Modifiers.Add(Modifier);
        return true;
    }

    bool RemoveModifier(const FStatModifier& Modifier)
    {
        return Modifiers.RemoveSingle(Modifier) > 0;
    }

    float GetFinalValue() const
    {
        return StatModOpHelper::Aggregate(BaseValue, Modifiers);
    }
};