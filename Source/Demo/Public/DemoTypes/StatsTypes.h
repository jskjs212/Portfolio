// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StatsTypes.generated.h"

/**
 * @TODO - Create a common parent for all stats that holds StatModifiers.
 */

UENUM()
enum class EStatModOp : uint8
{
    Add UMETA(DisplayName = "Additive"),
    Multiply UMETA(DisplayName = "Multiplicative"),
    Count UMETA(Hidden)
};

/**
 * Modifier format for buffs, debuffs, equipment, etc.
 * Consider adding: Duration, Handle(GUID)
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

// Helper function for Demo stats system.
namespace Demo::Stats
{
inline float Aggregate(float BaseLikeValue, const TArray<FStatModifier>& Modifiers)
{
    static_assert(static_cast<uint8>(EStatModOp::Count) == 2, "You added a new EStatModOp value, but didn't update the switch statements.");

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

        default: _UNLIKELY
            checkNoEntry();
            break;
        }
    }

    return (BaseLikeValue + ToAdd) * ToMultiply;
}
} // namespace Demo::Stats

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

    // [units per second]
    UPROPERTY(EditAnywhere, Category = "Stats", meta = (EditCondition = "bCanRegen", ClampMin = "0.0"))
    float RegenRate{5.f};

    // If 0, never regen. [seconds]
    UPROPERTY(EditAnywhere, Category = "Stats", meta = (EditCondition = "bCanRegen", ClampMin = "0.0", Units = "s"))
    float RegenInterval{0.1f};

    // [seconds]
    UPROPERTY(EditAnywhere, Category = "Stats", meta = (EditCondition = "bCanRegen", ClampMin = "0.0", Units = "s"))
    float RegenDelay{1.f};

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
};

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
        return Demo::Stats::Aggregate(BaseLike, Modifiers);
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
        return Demo::Stats::Aggregate(BaseValue, Modifiers);
    }
};