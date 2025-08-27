// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/TableRowBases.h"
#include "GameplayTagContainer.h"
#include "Items/ItemTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

/**
 * Validated data for internal functions' out parameters.
 */
USTRUCT()
struct FInventoryValidatedData
{
    GENERATED_BODY()

    FGameplayTag ItemCategory;

    FItemSlot* ItemSlot{nullptr};

    const FItemDataBase* ItemData{nullptr};

    TArray<FItemSlot>* ItemArray{nullptr};
};

/**
 * Inventory
 * Map of { ItemCategory, Array<ItemSlot> }
 * Fixed ItemCategories from DemoItemTypes::ItemCategories.
 * ItemSlot.Quantity == 0 means empty slot, although ItemSlot.RowHandle may be valid (not cleared when emptied).
 *
 * bFixSlotSizeAndExposeEmptySlots is true by default.
 * -> Each ItemCategory has its own ItemArray with fixed number (MaxSlotSize) of slots. Empty slots are shown.
 * -> If false, slots can be added/removed <= MaxSlotSize. Empty slots are not allowed.
 *
 * bAllowMultipleSlots is true by default.
 * -> Same items can take multiple slots.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Inventory functions
    ////////////////////////////////////////////////////////
public:
    // Add item to inventory.
    // For/while loops are used. Numbers should not be absurd (Size << 1000).
    // @param DesignatedIndex >=0 -> Top-up and discard overflow if same item exists, otherwise cancel.
    // @param DesignatedIndex  <0 -> Top-up existing slots, then fill empty slots.
    // @return Actually added quantity (subtract InSlot.Quantity), -1 if failed.
    int32 AddItem(FItemSlot& InSlot, int32 DesignatedIndex = -1);

    // Remove item from inventory.
    // @return Actually removed quantity, -1 if failed.
    int32 RemoveItem(const FItemActionRequest& Request);

    // Use item.
    // void UseItem(?);

    // Drop item.
    // void DropItem(?);

private:
    // Set initial max slot sizes, and fill with empty slots if bFixSlotSizeAndExposeEmptySlots == true.
    void InitMaxSlots();

    //
    // @return true if the slot is empty in inventory's perspective.
    FORCEINLINE bool IsInventorySlotEmpty(const FItemSlot& Slot) const
    {
        checkf(Slot.Quantity >= 0, TEXT("Negative quantity implies a broken code."));
        checkf(bFixSlotSizeAndExposeEmptySlots || Slot.Quantity != 0, TEXT("Empty slots are not allowed if bFixSlotSizeAndExposeEmptySlots is false."));
        return Slot.Quantity == 0;
    }

    // Validate InSlot for inventory, and get related data.
    // @param OutData is filled only if valid.
    // @return true if valid.
    bool ValidateInInventorySlot(const FItemSlot& InSlot, FInventoryValidatedData& OutData);

    // Internal function for adding item to inventory.
    // @return false if failed
    bool AddItem_Internal(FItemSlot& InSlot, int32 DesignatedIndex, int32 MaxStackSize, int32 MaxSlotSize, int32& InOutRemainingQuantity, TArray<FItemSlot>*& ItemArray);

    // Validate item action request, and get related data.
    // @param OutData is filled only if valid.
    // @return true if valid.
    bool ValidateActionRequest(const FItemActionRequest& Request, FInventoryValidatedData& OutData);

    ////////////////////////////////////////////////////////
    //        Variables - Inventory
    ////////////////////////////////////////////////////////
private:
    // { ItemCategory, Array<ItemSlot> }
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TMap<FGameplayTag, FItemArray> OwnedItems;

    // check: can be added by purchase?
    // If changed, this should be applied to OwnedItems if bFixSlotSizeAndExposeEmptySlots == true.
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TMap<FGameplayTag, int32> MaxSlotSizes;

    ////////////////////////////////////////////////////////
    //        Variables - General
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    bool bFixSlotSizeAndExposeEmptySlots{true};

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    bool bAllowMultipleSlots{true};
};