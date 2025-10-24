// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StatsComponent.h"
#include "Components/EquipmentComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/ItemTypes.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/TableRowBases.h"
#include "Items/Item.h"

const FGameplayTag UStatsComponent::HealthTag = FGameplayTag::RequestGameplayTag("Stat.Resource.Health");
const FGameplayTag UStatsComponent::StaminaTag = FGameplayTag::RequestGameplayTag("Stat.Resource.Stamina");
const FGameplayTag UStatsComponent::STRTag = FGameplayTag::RequestGameplayTag("Stat.Primary.STR");
const FGameplayTag UStatsComponent::DEXTag = FGameplayTag::RequestGameplayTag("Stat.Primary.DEX");
const FGameplayTag UStatsComponent::INTTag = FGameplayTag::RequestGameplayTag("Stat.Primary.INT");
const FGameplayTag UStatsComponent::AttackTag = FGameplayTag::RequestGameplayTag("Stat.Derived.Attack");
const FGameplayTag UStatsComponent::DefenseTag = FGameplayTag::RequestGameplayTag("Stat.Derived.Defense");

UStatsComponent::UStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UStatsComponent::InitStatsComponent()
{
    static const FGameplayTagContainer MustHaveStatType = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
        STRTag, DEXTag, INTTag, AttackTag, DefenseTag
    });
    for (FGameplayTag StatTag : MustHaveStatType)
    {
        if (!HasStatType(StatTag))
        {
            DemoLOG_F(LogAttributes, Error, TEXT("Stat %s is missing from %s."), *StatTag.GetTagName().ToString(), *GetNameSafe(GetOwner()));
        }
    }

    for (auto& [StatTag, ResourceStat] : ResourceStats)
    {
        ResourceStat.TimerDelegate.BindUObject(this, &ThisClass::RegenChecked, StatTag);
    }

    if (UEquipmentComponent* EquipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>())
    {
        EquipmentComponent->OnEquipped.AddUObject(this, &ThisClass::HandleItemEquipped);
        EquipmentComponent->OnUnequipped.AddUObject(this, &ThisClass::HandleItemUnequipped);
    }
}

void UStatsComponent::ResetAllResourceStats()
{
    for (auto& [StatTag, ResourceStat] : ResourceStats)
    {
        SetCurrentResourceStatChecked(StatTag, ResourceStat.MaxValue);
    }
}

void UStatsComponent::AddResourceStat(const FGameplayTag StatTag, const FResourceStat& ResourceStat)
{
    static const FGameplayTagContainer AllowedResouceStatTags = FGameplayTagContainer::CreateFromArray(TArray<FGameplayTag>{
        HealthTag, StaminaTag
    });

    if (!StatTag.MatchesAnyExact(AllowedResouceStatTags))
    {
        DemoLOG_F(LogAttributes, Error, TEXT("Stat %s is not allowed."), *StatTag.GetTagName().ToString());
        return;
    }

    if (ResourceStats.Contains(StatTag))
    {
        DemoLOG_F(LogAttributes, Warning, TEXT("Stat %s already exists."), *StatTag.GetTagName().ToString());
        return;
    }

    FResourceStat& ResourceStatRef = ResourceStats.Add(StatTag, ResourceStat);
    if (HasBegunPlay())
    {
        ResourceStatRef.TimerDelegate.BindUObject(this, &ThisClass::RegenChecked, /* Payload Data */ StatTag);
    }
}

void UStatsComponent::RemoveResourceStat(FGameplayTag StatTag)
{
    FResourceStat* ResourceStat = ResourceStats.Find(StatTag);
    if (!ResourceStat)
    {
        DemoLOG_F(LogAttributes, Warning, TEXT("Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
        return;
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(ResourceStat->TimerHandle);
    }
    // Unbind in ~TDelegateBase()
    ResourceStats.Remove(StatTag);
}

void UStatsComponent::AddPrimaryStat(FGameplayTag StatTag, const FPrimaryStat& PrimaryStat)
{
    if (PrimaryStats.Contains(StatTag))
    {
        DemoLOG_F(LogAttributes, Warning, TEXT("Stat %s already exists."), *StatTag.GetTagName().ToString());
        return;
    }
    PrimaryStats.Add(StatTag, PrimaryStat);
}

void UStatsComponent::AddDerivedStat(FGameplayTag StatTag, const FDerivedStat& DerivedStat)
{
    if (DerivedStats.Contains(StatTag))
    {
        DemoLOG_F(LogAttributes, Warning, TEXT("Stat %s already exists."), *StatTag.GetTagName().ToString());
        return;
    }
    DerivedStats.Add(StatTag, DerivedStat);
}

float UStatsComponent::ModifyCurrentResourceStatChecked(const FGameplayTag StatTag, const float Delta, const bool bShouldRegenerate, const float MinValue)
{
    if (Delta == 0.f)
    {
        return 0.f;
    }

    // Set timer before broadcast so listeners can stop it if needed.
    if (bShouldRegenerate)
    {
        StartRegenChecked(StatTag);
    }

    const float OldValue = GetCurrentResourceStatChecked(StatTag);
    const float NewValue = SetCurrentResourceStatChecked(StatTag, OldValue + Delta, MinValue);

    return NewValue - OldValue;
}

bool UStatsComponent::AddModifierToStat(FGameplayTag StatTag, const FStatModifier& Modifier)
{
    if (PrimaryStats.Contains(StatTag))
    {
        return AddModifierToPrimaryStat(StatTag, Modifier);
    }
    if (DerivedStats.Contains(StatTag))
    {
        return AddModifierToDerivedStat(StatTag, Modifier);
    }
    DemoLOG_CF(LogAttributes, Error, TEXT("Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
    return false;
}

bool UStatsComponent::RemoveModifierFromStat(FGameplayTag StatTag, const FStatModifier& Modifier)
{
    if (PrimaryStats.Contains(StatTag))
    {
        return RemoveModifierFromPrimaryStat(StatTag, Modifier);
    }
    if (DerivedStats.Contains(StatTag))
    {
        return RemoveModifierFromDerivedStat(StatTag, Modifier);
    }
    DemoLOG_CF(LogAttributes, Error, TEXT("Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
    return false;
}

bool UStatsComponent::AddModifierToPrimaryStat(FGameplayTag StatTag, const FStatModifier& Modifier)
{
    FPrimaryStat* PrimaryStat = PrimaryStats.Find(StatTag);
    if (!PrimaryStat)
    {
        DemoLOG_F(LogAttributes, Display, TEXT("Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
        return false;
    }

    const float OldValue = PrimaryStat->GetFinalValue();
    const bool bAdded = PrimaryStat->AddModifier(Modifier);
    if (bAdded)
    {
        const float NewValue = PrimaryStat->GetFinalValue();
        DemoLOG_CF(LogAttributes, Display, TEXT("Stat: %s, NewValue: %.2f"), *StatTag.GetTagName().ToString(), NewValue); // @debug
        OnPrimaryStatChanged.Broadcast(StatTag, OldValue, NewValue);
        RecalculateDerivedStat(StatTag);
        return true;
    }
    return false;
}

bool UStatsComponent::RemoveModifierFromPrimaryStat(FGameplayTag StatTag, const FStatModifier& Modifier)
{
    FPrimaryStat* PrimaryStat = PrimaryStats.Find(StatTag);
    if (!PrimaryStat)
    {
        DemoLOG_F(LogAttributes, Warning, TEXT("Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
        return false;
    }

    const float OldValue = PrimaryStat->GetFinalValue();
    const bool bRemoved = PrimaryStat->RemoveModifier(Modifier);
    if (bRemoved)
    {
        const float NewValue = PrimaryStat->GetFinalValue();
        DemoLOG_CF(LogAttributes, Display, TEXT("Stat: %s, NewValue: %.2f"), *StatTag.GetTagName().ToString(), NewValue); // @debug
        OnPrimaryStatChanged.Broadcast(StatTag, OldValue, NewValue);
        RecalculateDerivedStat(StatTag);
        return true;
    }
    return false;
}

bool UStatsComponent::AddModifierToDerivedStat(FGameplayTag StatTag, const FStatModifier& Modifier)
{
    FDerivedStat* DerivedStat = DerivedStats.Find(StatTag);
    if (!DerivedStat)
    {
        DemoLOG_F(LogAttributes, Warning, TEXT("Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
        return false;
    }

    const float OldValue = DerivedStat->GetFinalValue();
    const bool bAdded = DerivedStat->AddModifier(Modifier);
    if (bAdded)
    {
        const float NewValue = DerivedStat->GetFinalValue();
        DemoLOG_CF(LogAttributes, Display, TEXT("Stat: %s, NewValue: %.2f"), *StatTag.GetTagName().ToString(), NewValue); // @debug
        OnDerivedStatChanged.Broadcast(StatTag, OldValue, NewValue);
        return true;
    }
    return false;
}

bool UStatsComponent::RemoveModifierFromDerivedStat(FGameplayTag StatTag, const FStatModifier& Modifier)
{
    FDerivedStat* DerivedStat = DerivedStats.Find(StatTag);
    if (!DerivedStat)
    {
        DemoLOG_F(LogAttributes, Warning, TEXT("Stat %s doesn't exist."), *StatTag.GetTagName().ToString());
        return false;
    }

    const float OldValue = DerivedStat->GetFinalValue();
    const bool bRemoved = DerivedStat->RemoveModifier(Modifier);
    if (bRemoved)
    {
        const float NewValue = DerivedStat->GetFinalValue();
        DemoLOG_CF(LogAttributes, Display, TEXT("Stat: %s, NewValue: %.2f"), *StatTag.GetTagName().ToString(), NewValue); // @debug
        OnDerivedStatChanged.Broadcast(StatTag, OldValue, NewValue);
        return true;
    }
    return false;
}

void UStatsComponent::RecalculateDerivedStat(FGameplayTag InPrimaryStatTag)
{
    // @hardcoded - Formula is hardcoded for now.
    // @TODO - DataTable
    // Attack = STR * 0.5 + DEX * 0.1
    // Defense = STR * 0.1 + DEX * 0.3 + INT * 0.1
    // {DerivedStat, Array{PrimaryStat, Weight}}
    static const TMap<FGameplayTag, TArray<TPair<FGameplayTag, float>>> DerivedStatDependencies = {
        {AttackTag, {{STRTag, 0.5f}, {DEXTag, 0.1f}}},
        {DefenseTag, {{STRTag, 0.1f}, {DEXTag, 0.3f}, {INTTag, 0.1f}}}
    };

    // For each derived stat
    for (const auto& [DerivedStatTag, DependencyPairs] : DerivedStatDependencies)
    {
        bool bShouldRecalculate = false;
        for (const auto& [PrimaryStatTag, Weight] : DependencyPairs)
        {
            if (InPrimaryStatTag == PrimaryStatTag)
            {
                // Related
                bShouldRecalculate = true;
                break;
            }
        }
        if (!bShouldRecalculate)
        {
            // Not related
            continue;
        }

        FDerivedStat& DerivedStat = GetDerivedStatChecked(DerivedStatTag);
        const float OldDerivedValue = DerivedStat.GetFinalValue();

        float NewDerivedBaseValue = 0.f;
        for (const auto& [PrimaryStatTag, Weight] : DependencyPairs)
        {
            NewDerivedBaseValue += GetPrimaryStatChecked(PrimaryStatTag).GetFinalValue() * Weight;
        }

        DerivedStat.BaseValue = NewDerivedBaseValue;
        const float NewDerivedValue = DerivedStat.GetFinalValue();
        DemoLOG_CF(LogAttributes, Display, TEXT("Stat: %s, NewValue: %.2f"), *DerivedStatTag.GetTagName().ToString(), NewDerivedValue); // @debug
        OnDerivedStatChanged.Broadcast(DerivedStatTag, OldDerivedValue, NewDerivedValue);
    }
}

void UStatsComponent::StartRegenChecked(const FGameplayTag StatTag)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        DemoLOG_F(LogAttributes, Error, TEXT("No world."));
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
        DemoLOG_F(LogAttributes, Error, TEXT("No world."));
        return;
    }

    FResourceStat& ResourceStat = GetResourceStatChecked(StatTag);

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
            FResourceStat& ResourceStat = Pair.Value;
            TimerManager.ClearTimer(ResourceStat.TimerHandle);
        }
    }
}

void UStatsComponent::HandleItemEquipped(FGameplayTag EquipmentType, const FItemSlot& EquippedSlot)
{
    //if (EquipmentType == DemoGameplayTags::Item_Weapon)
    //{
    //    // Add all modifiers from new weapon.
    //    if (const FWeaponData* WeaponData = EquippedSlot.RowHandle.GetRow<FWeaponData>(TEXT("UStatsComponent::HandleItemEquipped")))
    //    {
    //        for (const auto& [StatTag, StatModifier] : WeaponData->StatModifiers)
    //        {
    //            AddModifierToStat(StatTag, StatModifier);
    //        }
    //    }
    //}
    //else if (EquipmentType.MatchesTag(DemoGameplayTags::Item_Armor))
    //{
    //    // Add all modifiers from new armor.
    //    if (const FArmorData* ArmorData = EquippedSlot.RowHandle.GetRow<FArmorData>(TEXT("UStatsComponent::HandleItemEquipped")))
    //    {
    //        for (const auto& [StatTag, StatModifier] : ArmorData->StatModifiers)
    //        {
    //            AddModifierToStat(StatTag, StatModifier);
    //        }
    //    }
    //}

    if (const FEquipmentData* EquipmentData = EquippedSlot.RowHandle.GetRow<FEquipmentData>(TEXT("UStatsComponent::HandleItemEquipped")))
    {
        // Add all modifiers from new equipment.
        for (const auto& [StatTag, StatModifier] : EquipmentData->StatModifiers)
        {
            AddModifierToStat(StatTag, StatModifier);
        }
    }
}

void UStatsComponent::HandleItemUnequipped(FGameplayTag EquipmentType, const FItemSlot& UnequippedSlot)
{
    //if (EquipmentType == DemoGameplayTags::Item_Weapon)
    //{
    //    // Remove all modifiers from previous weapon.
    //    if (const FWeaponData* WeaponData = UnequippedSlot.RowHandle.GetRow<FWeaponData>(TEXT("UStatsComponent::HandleItemUnequipped")))
    //    {
    //        for (const auto& [StatTag, StatModifier] : WeaponData->StatModifiers)
    //        {
    //            RemoveModifierFromStat(StatTag, StatModifier);
    //        }
    //    }
    //}
    //else if (EquipmentType.MatchesTag(DemoGameplayTags::Item_Armor))
    //{
    //    // Remove all modifiers from previous armor.
    //    if (const FArmorData* ArmorData = UnequippedSlot.RowHandle.GetRow<FArmorData>(TEXT("UStatsComponent::HandleItemUnequipped")))
    //    {
    //        for (const auto& [StatTag, StatModifier] : ArmorData->StatModifiers)
    //        {
    //            RemoveModifierFromStat(StatTag, StatModifier);
    //        }
    //    }
    //}

    if (const FEquipmentData* EquipmentData = UnequippedSlot.RowHandle.GetRow<FEquipmentData>(TEXT("UStatsComponent::HandleItemUnequipped")))
    {
        // Remove all modifiers from previous equipment.
        for (const auto& [StatTag, StatModifier] : EquipmentData->StatModifiers)
        {
            RemoveModifierFromStat(StatTag, StatModifier);
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

    // @debug
    DemoLOG_F(LogAttributes, Display, TEXT("%.2f"), Damage);
    return Damage;
}

void UStatsComponent::ConsumeStamina(float StaminaCost)
{
    if (HasStatType(StaminaTag))
    {
        ModifyCurrentResourceStatChecked(StaminaTag, -StaminaCost, true);
    }
}

bool UStatsComponent::HasEnoughOrNoRestriction(FGameplayTag StatTag, float Value) const
{
    if (const FResourceStat* ResourceStat = ResourceStats.Find(StatTag))
    {
        return ResourceStat->CurrentValue >= Value;
    }
    return true;
}

TPair<bool, float> UStatsComponent::GetStatFinalValue(FGameplayTag StatTag) const
{
    if (const FResourceStat* ResourceStat = ResourceStats.Find(StatTag))
    {
        return {true, ResourceStat->CurrentValue};
    }
    else if (const FPrimaryStat* PrimaryStat = PrimaryStats.Find(StatTag))
    {
        return {true, PrimaryStat->GetFinalValue()};
    }
    else if (const FDerivedStat* DerivedStat = DerivedStats.Find(StatTag))
    {
        return {true, DerivedStat->GetFinalValue()};
    }
    return {false, 0.f};
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
