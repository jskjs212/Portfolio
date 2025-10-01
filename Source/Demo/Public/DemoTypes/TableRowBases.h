// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoTypes/AttackTypes.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Misc/DataValidation.h"
#include "TableRowBases.generated.h"

class USkeletalMesh;
class UStaticMesh;
class UTexture2D;

// Consider adding:
// Price (Buy/Sell), Weight, Rarity, MaxDurability, bCanDrop
// bIsQuestItem, ItemSubType, Sound, Particle, Animation
// Requirements
USTRUCT(BlueprintType)
struct FItemDataBase : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName Name;

    UPROPERTY(EditAnywhere, meta = (Categories = "Item"))
    FGameplayTag ItemType;

    UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
    FText Description;

    UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
    int32 MaxStackSize{1};

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UStaticMesh> StaticMesh;

    UPROPERTY(EditAnywhere)
    TObjectPtr<USkeletalMesh> SkeletalMesh;

    //UPROPERTY(EditAnywhere)
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

// Category: Item.Weapon
// Consider adding: AttackSpeed, DamageType, Range, StatBonus
USTRUCT(BlueprintType)
struct FWeaponData : public FItemDataBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float Damage{10.f};

    UPROPERTY(EditAnywhere)
    FAttackCollisionDefinition AttackCollisionDefinition{.CollisionType = EAttackCollisionType::MainWeapon};

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
// Consider adding: StatBonus (MaxHealth), Resistance,
USTRUCT(BlueprintType)
struct FArmorData : public FItemDataBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere)
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

    UPROPERTY(EditAnywhere)
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