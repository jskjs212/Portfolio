// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoTypes/AttackTypes.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/StatsTypes.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // WITH_EDITOR

#include "ItemRowBases.generated.h"

class UParticleSystem;
class USkeletalMesh;
class USoundBase;
class UStaticMesh;
class UTexture2D;

// Consider adding:
// Tag to Attribute mapping
// Price (Buy/Sell), Weight, Rarity, MaxDurability, bCanDrop
// bIsQuestItem, ItemSubType, Sound, Particle, Animation
// Requirements
USTRUCT()
struct FItemDataBase : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    FName Name;

    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Item"))
    FGameplayTag ItemType;

    UPROPERTY(EditDefaultsOnly, meta = (MultiLine = "true"))
    FText Description;

    UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1"))
    int32 MaxStackSize{1};

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UStaticMesh> StaticMesh;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditDefaultsOnly)
    FVector MeshRelativeScale3D{FVector::OneVector};

    //UPROPERTY(EditDefaultsOnly)
    //TMap<FGameplayTag, float> Attributes;
    //
    //// @return { bFound, Value }
    //TPair<bool, float> GetAttributeValue(FGameplayTag AttributeTag) const
    //{
    //    if (const float* Value = Attributes.Find(AttributeTag))
    //    {
    //        return { true, *Value };
    //    }
    //    return { false, 0.f };
    //}

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
    {
        if (Name.IsNone() || MaxStackSize < 1)
        {
            Context.AddError(FText::FromString(TEXT("Name is None or MaxStackSize < 1.")));
            return EDataValidationResult::Invalid;
        }
        if (!Icon)
        {
            Context.AddError(FText::FromString(TEXT("Icon is not valid.")));
            return EDataValidationResult::Invalid;
        }
        if (!StaticMesh && !SkeletalMesh)
        {
            Context.AddError(FText::FromString(TEXT("Mesh is not valid.")));
            return EDataValidationResult::Invalid;
        }
        return EDataValidationResult::Valid;
    }
#endif // WITH_EDITOR
};

USTRUCT()
struct FEquipmentData : public FItemDataBase
{
    GENERATED_BODY()

    // {StatTag, StatModifier}
    // Recommended StatModifier::SourceTag = EquipmentTypes like Item.Armor.Shield.
    UPROPERTY(EditDefaultsOnly)
    TMap<FGameplayTag, FStatModifier> StatModifiers;
};

// Category: Item.Weapon
// Consider adding: AttachSocketName, AttackSpeed, DamageType, Range, StatBonus
USTRUCT()
struct FWeaponData : public FEquipmentData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    FAttackCollisionDefinition AttackCollisionDefinition{.CollisionType = EAttackCollisionType::MainWeapon};

    FWeaponData()
    {
        StatModifiers = TMap<FGameplayTag, FStatModifier>{
            {FGameplayTag::RequestGameplayTag("Stat.Derived.Attack"), FStatModifier{EStatModOp::Add, 10.f, DemoGameplayTags::Item_Weapon}}
        };
    }

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
    {
        Super::IsDataValid(Context);

        if (!ItemType.MatchesTag(DemoGameplayTags::Item_Weapon))
        {
            Context.AddError(FText::FromString(TEXT("ItemType is not Weapon.")));
            return EDataValidationResult::Invalid;
        }
        if (!AttackCollisionDefinition.IsValid())
        {
            Context.AddError(FText::FromString(TEXT("AttackCollisionDefinition is not valid.")));
            return EDataValidationResult::Invalid;
        }
        if (SkeletalMesh)
        {
            for (const FAttackCollisionSegment& Segment : AttackCollisionDefinition.Segments)
            {
                if (!SkeletalMesh->FindSocket(Segment.StartSocketName) || !SkeletalMesh->FindSocket(Segment.EndSocketName))
                {
                    Context.AddError(FText::FromString(FString::Printf(TEXT("SkeletalMesh does not have socket: %s or %s."),
                        *Segment.StartSocketName.ToString(), *Segment.EndSocketName.ToString())));
                    return EDataValidationResult::Invalid;
                }
            }
        }
        if (StaticMesh)
        {
            for (const FAttackCollisionSegment& Segment : AttackCollisionDefinition.Segments)
            {
                if (!StaticMesh->FindSocket(Segment.StartSocketName) || !StaticMesh->FindSocket(Segment.EndSocketName))
                {
                    Context.AddError(FText::FromString(FString::Printf(TEXT("StaticMesh does not have socket: %s or %s."),
                        *Segment.StartSocketName.ToString(), *Segment.EndSocketName.ToString())));
                    return EDataValidationResult::Invalid;
                }
            }
        }
        return EDataValidationResult::Valid;
    }
#endif // WITH_EDITOR
};

// Category: Item.Armor
// Consider adding: AttachSocketName, StatBonus (MaxHealth), Resistance,
USTRUCT()
struct FArmorData : public FEquipmentData
{
    GENERATED_BODY()

    FArmorData()
    {
        StatModifiers = TMap<FGameplayTag, FStatModifier>{
            {FGameplayTag::RequestGameplayTag("Stat.Derived.Defense"), FStatModifier{EStatModOp::Add, 5.f, FGameplayTag::EmptyTag}}
        };
    }

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
    {
        Super::IsDataValid(Context);

        if (!ItemType.MatchesTag(DemoGameplayTags::Item_Armor))
        {
            Context.AddError(FText::FromString(TEXT("ItemType is not Armor.")));
            return EDataValidationResult::Invalid;
        }
        return EDataValidationResult::Valid;
    }
#endif // WITH_EDITOR
};

// Category: Item.Consumable
// Consider adding: Buff, Debuff, Cooldown, Duration
USTRUCT()
struct FConsumableData : public FItemDataBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    float HealAmount{10.f};

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<UParticleSystem> Effect;

    UPROPERTY(EditDefaultsOnly)
    TSoftObjectPtr<USoundBase> Sound;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
    {
        Super::IsDataValid(Context);

        if (!ItemType.MatchesTag(DemoGameplayTags::Item_Consumable))
        {
            Context.AddError(FText::FromString(TEXT("ItemType is not Consumable.")));
            return EDataValidationResult::Invalid;
        }
        return EDataValidationResult::Valid;
    }
#endif // WITH_EDITOR
};