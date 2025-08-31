// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Misc/DataValidation.h"
#include "TableRowBases.generated.h"

class USkeletalMesh;
class UStaticMesh;
class UTexture2D;

// Consider adding:
// Price (Buy/Sell), Weight, Rarity, MaxDurability
// bIsQuestItem, ItemSubType, Sound, Particle, Animation
// Requirements
USTRUCT(BlueprintType)
struct FItemDataBase : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    FName Name;

    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Item"))
    FGameplayTag ItemType;

    UPROPERTY(EditDefaultsOnly, meta = (MultiLine = "true"))
    FText Description;

    UPROPERTY(EditDefaultsOnly)
    int32 MaxStackSize{1};

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UStaticMesh> StaticMesh;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<USkeletalMesh> SkeletalMesh;

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
        if (!StaticMesh && !SkeletalMesh)
        {
            Context.AddError(FText::FromString(TEXT("Mesh is not valid.")));
            return EDataValidationResult::Invalid;
        }
        return EDataValidationResult::Valid;
    }
#endif // WITH_EDITOR
};

// Category: Item.Weapon
// Consider adding: AttackSpeed, DamageType, Range, StatBonus
USTRUCT(BlueprintType)
struct FWeaponData : public FItemDataBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    float Damage{10.f};

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
    {
        Super::IsDataValid(Context);

        if (!ItemType.MatchesTag(DemoGameplayTags::Item_Weapon))
        {
            Context.AddError(FText::FromString(TEXT("ItemType is not Weapon.")));
            return EDataValidationResult::Invalid;
        }
        return EDataValidationResult::Valid;
    }
#endif // WITH_EDITOR
};

// Category: Item.Armor
// Consider adding: StatBonus (MaxHealth), Resistance,
USTRUCT(BlueprintType)
struct FArmorData : public FItemDataBase
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly)
    float Armor{10.f};

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
USTRUCT(BlueprintType)
struct FConsumableData : public FItemDataBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    float HealAmount{10.f};

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