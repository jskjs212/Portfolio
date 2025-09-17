// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/StatsTypes.h"
#include "GameplayTagContainer.h"
#include "StatsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCurrentStatChanged, FGameplayTag /* StatTag */, float /* OldValue */, float /* NewValue */);

/**
 * Checked functions with invalid StatTags cause a crash!
 * If you want to handle the case of invalid stat type, use HasStatType() first.
 * i.e. When you don't know if hit actor has Mana stat or not.
 * Current values are not allowed to exceed max values.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UStatsComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Statics
    ////////////////////////////////////////////////////////
public:
    static const FGameplayTag HealthTag;
    static const FGameplayTag StaminaTag;

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnCurrentStatChanged OnCurrentResourceStatChanged;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UStatsComponent();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Stats
    ////////////////////////////////////////////////////////
public:
    // Set all current values to max values.
    void InitializeResourceStats();

    // Not considered to be called after BeginPlay.
    void AddResourceStat(FGameplayTag StatTag, const FResourceStat& ResourceStat);
    void RemoveResourceStat(FGameplayTag StatTag);

    // Current += Delta.
    // Don't handle with specific cases like Health <= 0.
    // @return The actual amount by which the value was modified.
    float ModifyCurrentResourceStatChecked(FGameplayTag StatTag, float Delta, bool bShouldRegenerate = false, float MinValue = 0.f);

    void StartRegenChecked(FGameplayTag StatTag);

    void RegenChecked(FGameplayTag StatTag);

    void StopAllRegen();

    ////////////////////////////////////////////////////////
    //        Wrapper functions
    ////////////////////////////////////////////////////////
public:
    // Health tag needed.
    FORCEINLINE float GetCurrentHealth() const { return GetCurrentResourceStatChecked(HealthTag); }

    // Health tag needed.
    FORCEINLINE float GetMaxHealth() const { return GetMaxResourceStatChecked(HealthTag); }

    // Health tag needed.
    FORCEINLINE float Heal(float HealAmount) { return ModifyCurrentResourceStatChecked(HealthTag, HealAmount, false); }

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
    FORCEINLINE bool HasStatType(FGameplayTag StatTag) const { return ResourceStats.Contains(StatTag); }

    // If the actor has no StatTag, return true (no restriction).
    // Otherwise, return Current >= Value.
    bool HasEnoughOrNoRestriction(FGameplayTag StatTag, float Value) const;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE float GetCurrentResourceStatChecked(FGameplayTag StatTag) const
    {
        return GetResourceStatChecked(StatTag).CurrentValue;
    }

    FORCEINLINE float GetMaxResourceStatChecked(FGameplayTag StatTag) const
    {
        return GetResourceStatChecked(StatTag).MaxValue;
    }

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

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Stats", meta = (Categories = "Stats"))
    TMap<FGameplayTag, FResourceStat> ResourceStats;
};