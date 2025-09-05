// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "StatsComponent.generated.h"

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

    ////////////////////////////////////////////////////////
    //        Wrapper functions (only health for now)
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE float GetCurrentHealth() const { return GetCurrentResourceStatChecked(HealthTag); }

    FORCEINLINE float GetMaxHealth() const { return GetMaxResourceStatChecked(HealthTag); }

    float Heal(float HealAmount) { return ModifyCurrentResourceStatChecked(HealthTag, HealAmount, false); }

    // @return The actual amount by which the damage was taken.
    float TakeDamage(float InDamage);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE bool HasStatType(FGameplayTag StatTag) const
    {
        return ResourceStats.Contains(StatTag);
    }

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