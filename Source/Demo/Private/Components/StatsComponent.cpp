// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StatsComponent.h"

const FGameplayTag UStatsComponent::HealthTag = FGameplayTag::RequestGameplayTag(TEXT("Stats.Health"));
const FGameplayTag UStatsComponent::StaminaTag = FGameplayTag::RequestGameplayTag(TEXT("Stats.Stamina"));

UStatsComponent::UStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UStatsComponent::BeginPlay()
{
    Super::BeginPlay();

    for (auto& [StatTag, ResourceStat] : ResourceStats)
    {
        ResourceStat.TimerDelegate.BindUObject(this, &ThisClass::RegenChecked, StatTag);
    }
}

void UStatsComponent::InitializeResourceStats()
{
    for (auto& [StatTag, ResourceStat] : ResourceStats)
    {
        SetCurrentResourceStatChecked(StatTag, ResourceStat.MaxValue);
    }
}

void UStatsComponent::AddResourceStat(FGameplayTag StatTag, FResourceStat ResourceStat)
{
    if (ResourceStats.Contains(StatTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("UStatsComponent::AddResourceStat - Stat %s already exists."), *StatTag.GetTagName().ToString());
        return;
    }
    ResourceStats.Add(StatTag, ResourceStat);
}

float UStatsComponent::ModifyCurrentStatChecked(FGameplayTag StatTag, float Delta, bool bShouldRegenerate, float MinValue)
{
    if (Delta == 0.f)
    {
        return 0.f;
    }

    const float OldValue = GetCurrentResourceStatChecked(StatTag);
    const float NewValue = SetCurrentResourceStatChecked(StatTag, OldValue + Delta, MinValue);

    if (bShouldRegenerate)
    {
        StartRegenChecked(StatTag);
    }

    return NewValue - OldValue;
}

void UStatsComponent::StartRegenChecked(FGameplayTag StatTag)
{
    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);

    if (ResourceStat.bCanRegen && ResourceStat.RegenInterval > 0.f && ResourceStat.RegenDelay > 0.f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            ResourceStat.TimerHandle,
            ResourceStat.TimerDelegate,
            ResourceStat.RegenInterval,
            true,
            ResourceStat.RegenDelay);
    }
}

void UStatsComponent::RegenChecked(FGameplayTag StatTag)
{
    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);

    // TODO: Dead

    // Regen
    ModifyCurrentStatChecked(StatTag, ResourceStat.RegenRate * ResourceStat.RegenInterval, false);

    // Full
    if (ResourceStat.CurrentValue >= ResourceStat.MaxValue)
    {
        GetWorld()->GetTimerManager().ClearTimer(ResourceStat.TimerHandle);
        return;
    }
}

float UStatsComponent::TakeDamage(float InDamage)
{
    // Damage calculation
    float Damage = InDamage;
    // Damage = Damage * (Damage / (Damage + Armor))

    constexpr float MinDamage = 0.f;
    constexpr float MaxDamage = 1e10f; // misc: Set max damage
    Damage = FMath::Clamp(Damage, MinDamage, MaxDamage);

    Damage = ModifyCurrentStatChecked(HealthTag, -Damage, true) * -1.f;

    // TODO: death?

    // TEST:
    UE_LOG(LogTemp, Warning, TEXT("UStatsComponent::TakeDamage - %.2f"), Damage);
    return Damage;
}

float UStatsComponent::SetCurrentResourceStatChecked(FGameplayTag StatTag, float InValue, float MinValue)
{
    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);
    const float OldValue = ResourceStat.CurrentValue;
    const float NewValue = FMath::Clamp(InValue, MinValue, ResourceStat.MaxValue);

    if (OldValue == NewValue)
    {
        return OldValue;
    }

    // TEST:
    UE_LOG(LogTemp, Warning, TEXT("UStatsComponent - Set current %s to %.2f"), *StatTag.GetTagName().ToString(), NewValue);

    ResourceStat.CurrentValue = NewValue;
    OnCurrentResourceStatChanged.Broadcast(StatTag, OldValue, NewValue);

    return NewValue;
}

void UStatsComponent::SetMaxResourceStatChecked(FGameplayTag StatTag, float InValue, float MinValue)
{
    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);
    ResourceStat.MaxValue = InValue;

    if (ResourceStat.CurrentValue > InValue)
    {
        SetCurrentResourceStatChecked(StatTag, InValue, MinValue);
    }
}
