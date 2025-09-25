// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.generated.h"

// Defines item categories for Demo's item & inventory system.
// ItemCategory: Item.Weapon, Item.Armor, Item.Consumable
// ItemType: Item.Weapon.Melee.OneHanded, Item.Consumable.Food, etc.
// EquipmentType: Item.Armor.Shield, etc.
namespace DemoItemTypes
{
// Inventory TMap's keys.
// MUST sync with InventoryPageWidget's tab entries.
// @check - What if I add an InventoryComponent to containers like Chest, Vendor, etc.?
//   -> Different ItemCategories for different container types???
FORCEINLINE const TArray<const FGameplayTag>& GetItemCategories()
{
    static const TArray<const FGameplayTag> ItemCategories{
        DemoGameplayTags::Item_Weapon,
        DemoGameplayTags::Item_Armor,
        DemoGameplayTags::Item_Consumable
        // or Tool, Quest, Material, etc.
    };
    return ItemCategories;
}

// Equipment TMap's keys.
// MUST sync with EquipmentPageWidget's slots.
FORCEINLINE const TArray<const FGameplayTag>& GetEquipmentTypes()
{
    static const TArray<const FGameplayTag> EquipmentTypes{
        DemoGameplayTags::Item_Weapon,
        DemoGameplayTags::Item_Armor_Shield
        // or Helmet, Boots, etc.
    };
    return EquipmentTypes;
}

// Find item category from item type.
// ex) Item.Weapon.Melee.OneHanded -> Item.Weapon
// @return EmptyTag if not found.
FORCEINLINE FGameplayTag GetItemCategory(const FGameplayTag ItemType)
{
    for (const FGameplayTag& ItemCategory : GetItemCategories())
    {
        if (ItemType.MatchesTag(ItemCategory))
        {
            return ItemCategory;
        }
    }
    return FGameplayTag::EmptyTag;
}

// Find equipment type from item type.
// @return EmptyTag if not found.
FORCEINLINE FGameplayTag GetEquipmentType(const FGameplayTag ItemType)
{
    for (const FGameplayTag& EquipmentType : GetEquipmentTypes())
    {
        if (ItemType.MatchesTag(EquipmentType))
        {
            return EquipmentType;
        }
    }
    return FGameplayTag::EmptyTag;
}
} // namespace DemoItemTypes

/**
 * Item slot for inventory & UI.
 */
USTRUCT(BlueprintType)
struct FItemSlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Item")
    FDataTableRowHandle RowHandle;

    UPROPERTY(EditAnywhere, Category = "Item")
    int32 Quantity{0};

    UPROPERTY(EditAnywhere, Category = "Item")
    bool bIsLocked{false};

    // We need only quantity at the moment.
    // Scalability example:
    //     FItemInstanceData (FGuid ItemID, ExpiryDate, etc.)
    //     FWeaponInstanceData (Durability, Enchant, MasteryLevel, Attachment, etc.)
    // Maybe TArray<TObjectPtr<UItemFragment>> ItemFragments?
    // Or maybe inherit from FItemSlot itself?
    //UPROPERTY(VisibleAnywhere)
    //TObjectPtr<FItemInstanceData> InstanceData;

    // @return true if RowHandle is valid and Quantity > 0.
    FORCEINLINE bool IsValid() const { return !RowHandle.IsNull() && Quantity > 0; }
};

/**
 * Item array for each item category.
 */
USTRUCT(BlueprintType)
struct FItemArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<FItemSlot> ItemArray;
};

/**
 * Contains data for inventory operations such as Remove, Use, Drop
 */
USTRUCT()
struct FItemActionRequest
{
    GENERATED_BODY()

    FItemSlot Slot;

    // Index in inventory's item array.
    int32 DesignatedIndex{-1};

    int32 Quantity{0};
};