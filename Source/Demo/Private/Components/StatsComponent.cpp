// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"

const FGameplayTag UStatsComponent::HealthTag = FGameplayTag::RequestGameplayTag("Stat.Health");
const FGameplayTag UStatsComponent::StaminaTag = FGameplayTag::RequestGameplayTag("Stat.Stamina");

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

void UStatsComponent::AddResourceStat(const FGameplayTag StatTag, const FResourceStat& ResourceStat)
{
    if (ResourceStats.Contains(StatTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("UStatsComponent::AddResourceStat - Stat %s already exists."), *StatTag.GetTagName().ToString());
        return;
    }

    FResourceStat& ResourceStatRef = ResourceStats.Add(StatTag, ResourceStat);
    if (GetWorld())
    {
        // Just in case added after BeginPlay
        ResourceStatRef.TimerDelegate.BindUObject(this, &ThisClass::RegenChecked, /* Payload Data */ StatTag);
    }
}

void UStatsComponent::RemoveResourceStat(FGameplayTag StatTag)
{
    FResourceStat* ResourceStat = ResourceStats.Find(StatTag);
    if (!ResourceStat)
    {
        UE_LOG(LogTemp, Warning, TEXT("UStatsComponent::RemoveResourceStat - Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
        return;
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ResourceStat->TimerHandle);
    }
    // Unbind in ~TDelegateBase()
    ResourceStats.Remove(StatTag);
}

float UStatsComponent::ModifyCurrentResourceStatChecked(const FGameplayTag StatTag, const float Delta, const bool bShouldRegenerate, const float MinValue)
{
    if (Delta == 0.f)
    {
        return 0.f;
    }

    // Set timer before broadcast
    if (bShouldRegenerate)
    {
        StartRegenChecked(StatTag);
    }

    const float OldValue = GetCurrentResourceStatChecked(StatTag);
    const float NewValue = SetCurrentResourceStatChecked(StatTag, OldValue + Delta, MinValue);

    return NewValue - OldValue;
}

void UStatsComponent::StartRegenChecked(const FGameplayTag StatTag)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("UStatsComponent::StartRegenChecked - No world."));
        return;
    }

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

void UStatsComponent::RegenChecked(const FGameplayTag StatTag)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("UStatsComponent::RegenChecked - No world."));
        return;
    }

    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);

    // @TODO - Dead

    // Regen
    ModifyCurrentResourceStatChecked(StatTag, ResourceStat.RegenRate * ResourceStat.RegenInterval, false);

    // Full
    if (ResourceStat.CurrentValue >= ResourceStat.MaxValue)
    {
        GetWorld()->GetTimerManager().ClearTimer(ResourceStat.TimerHandle);
        return;
    }
}

void UStatsComponent::StopAllRegen()
{
    if (UWorld* World = GetWorld())
    {
        FTimerManager& TimerManager = World->GetTimerManager();
        for (TPair<FGameplayTag, FResourceStat>& Pair : ResourceStats)
        {
            UE_LOG(LogTemp, Warning, TEXT("UStatsComponent::StopAllRegen - Stopping regen for %s"), *Pair.Key.GetTagName().ToString());
            FResourceStat& ResourceStat = Pair.Value;
            TimerManager.ClearTimer(ResourceStat.TimerHandle);
        }
    }
}

float UStatsComponent::TakeDamage(const float InDamage)
{
    // Damage calculation
    float Damage = InDamage;
    // Damage = Damage * (Damage / (Damage + Armor))

    constexpr float MinDamage = 0.f;
    constexpr float MaxDamage = 1e10f; // @misc - Set max damage
    Damage = FMath::Clamp(Damage, MinDamage, MaxDamage);

    Damage = ModifyCurrentResourceStatChecked(HealthTag, -Damage, true) * -1.f;

    // @TODO - death?

    // @debug
    UE_LOG(LogTemp, Display, TEXT("UStatsComponent::TakeDamage - %.2f"), Damage);
    return Damage;
}

bool UStatsComponent::HasEnough(FGameplayTag StatTag, float Value) const
{
    if (const FResourceStat* ResourceStat = ResourceStats.Find(StatTag))
    {
        return ResourceStat->CurrentValue >= Value;
    }
    return false;
}

float UStatsComponent::SetCurrentResourceStatChecked(const FGameplayTag StatTag, const float InValue, const float MinValue)
{
    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);
    const float OldValue = ResourceStat.CurrentValue;
    const float NewValue = FMath::Clamp(InValue, MinValue, ResourceStat.MaxValue);

    if (OldValue == NewValue)
    {
        return OldValue;
    }

    ResourceStat.CurrentValue = NewValue;
    OnCurrentResourceStatChanged.Broadcast(StatTag, OldValue, NewValue);

    return NewValue;
}

void UStatsComponent::SetMaxResourceStatChecked(const FGameplayTag StatTag, const float InValue, const float MinValue)
{
    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);
    ResourceStat.MaxValue = InValue;

    if (ResourceStat.CurrentValue > InValue)
    {
        SetCurrentResourceStatChecked(StatTag, InValue, MinValue);
    }
}
