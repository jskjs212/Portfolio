// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/StatsTypes.h"
#include "GameplayTagContainer.h"
#include "StatsComponent.generated.h"

struct FItemSlot;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnStatChanged, FGameplayTag /* StatTag */, float /* OldValue */, float /* NewValue */);

/**
 * Checked functions with invalid StatTags cause a crash!
 * If you want to handle the case of invalid stat type, use HasStatType() first.
 * e.g. When you don't know if hit actor has Mana stat or not.
 * Current values are not allowed to exceed max values.
 * Call InitStatsComponent() after adding all stats.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UStatsComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnStatChanged OnCurrentResourceStatChanged;
    FOnStatChanged OnPrimaryStatChanged;
    FOnStatChanged OnDerivedStatChanged;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UStatsComponent();

    ////////////////////////////////////////////////////////
    //        Stats
    ////////////////////////////////////////////////////////
public:
    void InitStatsComponent();

    // Set all current values to max values.
    void ResetAllResourceStats();

    // Not considered to be called after BeginPlay.
    void AddResourceStat(FGameplayTag StatTag, const FResourceStat& ResourceStat);
    void RemoveResourceStat(FGameplayTag StatTag);
    void AddPrimaryStat(FGameplayTag StatTag, const FPrimaryStat& PrimaryStat);
    void AddDerivedStat(FGameplayTag StatTag, const FDerivedStat& DerivedStat);

    // Current += Delta.
    // Don't handle with specific cases like Health <= 0.
    // @return The actual amount by which the value was modified.
    float ModifyCurrentResourceStatChecked(FGameplayTag StatTag, float Delta, bool bShouldRegenerate = false, float MinValue = 0.f);

    bool AddModifierToStat(FGameplayTag StatTag, const FStatModifier& Modifier);
    bool RemoveModifierFromStat(FGameplayTag StatTag, const FStatModifier& Modifier);
    bool AddModifierToPrimaryStat(FGameplayTag StatTag, const FStatModifier& Modifier);
    bool RemoveModifierFromPrimaryStat(FGameplayTag StatTag, const FStatModifier& Modifier);
    bool AddModifierToDerivedStat(FGameplayTag StatTag, const FStatModifier& Modifier);
    bool RemoveModifierFromDerivedStat(FGameplayTag StatTag, const FStatModifier& Modifier);

    // Should be called after modifying primary stats!
    void RecalculateDerivedStat(FGameplayTag InPrimaryStatTag);

    void StartRegenChecked(FGameplayTag StatTag);

    void RegenChecked(FGameplayTag StatTag);

    void StopAllRegen();

private:
    void HandleItemEquipped(FGameplayTag EquipmentType, const FItemSlot& EquippedSlot);
    void HandleItemUnequipped(FGameplayTag EquipmentType, const FItemSlot& UnequippedSlot);

    ////////////////////////////////////////////////////////
    //        Wrapper functions
    ////////////////////////////////////////////////////////
public:
    // Health tag needed.
    FORCEINLINE float GetCurrentHealth() const { return GetCurrentResourceStatChecked(DemoGameplayTags::Stat_Resource_Health); }

    // Health tag needed.
    FORCEINLINE float GetMaxHealth() const { return GetMaxResourceStatChecked(DemoGameplayTags::Stat_Resource_Health); }

    // Health tag needed.
    FORCEINLINE float Heal(float HealAmount) { return ModifyCurrentResourceStatChecked(DemoGameplayTags::Stat_Resource_Health, HealAmount, false); }

    // Health tag needed.
    // @return The actual amount by which the damage was taken.
    float TakeDamage(float InDamage);

    // Consume stamina whether it's enough or not.
    // Do nothing if there's no Stamina tag.
    void ConsumeStamina(float StaminaCost);

    ////////////////////////////////////////////////////////
    //        Get & set (all stat types)
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE bool HasStatType(FGameplayTag StatTag) const
    {
        return ResourceStats.Contains(StatTag) || PrimaryStats.Contains(StatTag) || DerivedStats.Contains(StatTag);
    }

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    // If the actor has no StatTag, return true (no restriction).
    // Otherwise, return Current >= Value.
    bool HasEnoughOrNoRestriction(FGameplayTag StatTag, float Value) const;

    FORCEINLINE float GetCurrentResourceStatChecked(FGameplayTag StatTag) const
    {
        return GetResourceStatChecked(StatTag).CurrentValue;
    }

    FORCEINLINE float GetMaxResourceStatChecked(FGameplayTag StatTag) const
    {
        return GetResourceStatChecked(StatTag).MaxValue;
    }

    FORCEINLINE float GetPrimaryStatFinalValueChecked(FGameplayTag StatTag) const
    {
        return GetPrimaryStatChecked(StatTag).GetFinalValue();
    }

    FORCEINLINE float GetDerivedStatFinalValueChecked(FGameplayTag StatTag) const
    {
        return GetDerivedStatChecked(StatTag).GetFinalValue();
    }

    // Get current resource stat or primary/derived stat final value.
    // @return {bFound, Value}
    TPair<bool, float> GetStatFinalValue(FGameplayTag StatTag) const;

    // Don't handle with specific cases like Health <= 0.
    // @return The new value of the stat.
    float SetCurrentResourceStatChecked(FGameplayTag StatTag, float InValue, float MinValue = 0.f);

    void SetMaxResourceStatChecked(FGameplayTag StatTag, float InValue, float MinValue = 0.f);

private:
    // Assume that no one uses an invalid tag for the StatTag.
    // So it will not be checked in shipping build.
    FORCEINLINE const FResourceStat& GetResourceStatChecked(FGameplayTag StatTag) const
    {
        //const FResourceStat* ResourceStat = ResourceStats.Find(StatTag);
        //checkf(ResourceStat, TEXT("StatsComponent - Invalid tag is used for query."));
        //return ResourceStat;
        // TMap::operator[] does the exact same thing.
        return ResourceStats[StatTag];
    }

    FORCEINLINE FResourceStat& GetResourceStatChecked(FGameplayTag StatTag)
    {
        // (Practice) This is safe because "this" and member variables are non-const.
        return const_cast<FResourceStat&>(const_cast<const UStatsComponent*>(this)->GetResourceStatChecked(StatTag));
    }

    FORCEINLINE const FPrimaryStat& GetPrimaryStatChecked(FGameplayTag StatTag) const { return PrimaryStats[StatTag]; }
    FORCEINLINE FPrimaryStat& GetPrimaryStatChecked(FGameplayTag StatTag) { return PrimaryStats[StatTag]; }
    FORCEINLINE const FDerivedStat& GetDerivedStatChecked(FGameplayTag StatTag) const { return DerivedStats[StatTag]; }
    FORCEINLINE FDerivedStat& GetDerivedStatChecked(FGameplayTag StatTag) { return DerivedStats[StatTag]; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    TMap<FGameplayTag, FResourceStat> ResourceStats;

    UPROPERTY(VisibleAnywhere, Category = "Stats")
    TMap<FGameplayTag, FPrimaryStat> PrimaryStats;

    // Base value is modified only via RecalculateDerivedStat() by PrimaryStats.
    UPROPERTY(VisibleAnywhere, Category = "Stats")
    TMap<FGameplayTag, FDerivedStat> DerivedStats;
};