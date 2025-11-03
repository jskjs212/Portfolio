// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "Items/ItemRowBases.h"
#include "InventoryComponent.generated.h"

class UEquipmentComponent;
class UStatsComponent;

/**
 * Used to return validated data from internal functions.
 */
struct FInventoryValidationResult
{
    bool bIsValid{false};

    FGameplayTag ItemCategory;

    FItemSlot* ItemSlot{nullptr};

    const FItemDataBase* ItemData{nullptr};

    TArray<FItemSlot>* ItemArray{nullptr};
};

/**
 * Inventory
 * Map of { ItemCategory, Array<ItemSlot> }
 * Fixed ItemCategories from Demo::Item::ItemCategories.
 * ItemSlot.Quantity == 0 means empty slot, although ItemSlot.RowHandle may be valid (not cleared when emptied).
 * For/while loops are used in AddItem. Sizes should not be absurd (Size << 1000).
 *
 * bFixSlotSizeAndExposeEmptySlots is true by default.
 * -> Each ItemCategory has its own ItemArray with fixed number (MaxSlotSize) of slots. Empty slots are shown.
 * -> If false, slots can be added/removed <= MaxSlotSize. Empty slots are not allowed.
 *
 * bAllowMultipleSlots is true by default.
 * -> Same items can take multiple slots.
 *
 * @Dependency - Some ItemTypes in UseItem() need EquipmentComponent and StatsComponent.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FSimpleMulticastDelegate OnInventoryUpdated;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Inventory functions
    ////////////////////////////////////////////////////////
public:
    // Add item to inventory.
    // Caller should subtract InSlot.Quantity if needed.
    // @param DesignatedIndex >=0 -> Empty or same item: Top-up and ignore remainder.
    // @param DesignatedIndex >=0 -> Different item: Cancel.
    // @param DesignatedIndex  <0 -> Top-up existing slots, then fill empty slots.
    // @return Actually added quantity, -1 if failed.
    int32 AddItem(const FItemActionRequest& Request);

    // Remove item from inventory.
    // @return Actually removed quantity, -1 if failed.
    int32 RemoveItem(const FItemActionRequest& Request);

    // Use item from inventory.
    // @return Actually used quantity, -1 if failed.
    int32 UseItem(const FItemActionRequest& Request);

    // Drop item from inventory.
    // @return Actually dropped quantity, -1 if failed.
    int32 DropItem(const FItemActionRequest& Request);

    // Swap item between two inventory slots.
    // @return true if succeeded.
    bool SwapItem(FGameplayTag ItemCategory, int32 FirstIndex, int32 SecondIndex);

    // @return true if succeeded.
    bool AddMaxSlotSize(FGameplayTag ItemCategory, int32 ToAdd);

    ////////////////////////////////////////////////////////
    //        Inventory helper functions
    ////////////////////////////////////////////////////////
private:
    void InitInventoryComponent();

    // Set initial max slot sizes, and fill with empty slots if bFixSlotSizeAndExposeEmptySlots == true.
    void InitMaxSlots();

    // Bind inventory functions to UI's item action dispatcher.
    void BindToItemActionDispatcher();

    // @return true if the slot is empty in inventory's perspective.
    FORCEINLINE bool IsInventorySlotEmpty(const FItemSlot& Slot) const
    {
        checkf(Slot.Quantity >= 0, TEXT("Negative quantity implies a broken code."));
        checkf(bFixSlotSizeAndExposeEmptySlots || Slot.Quantity != 0, TEXT("Empty slots are not allowed if bFixSlotSizeAndExposeEmptySlots is false."));
        return Slot.Quantity == 0;
    }

    // Validate InSlot for inventory, and get related data.
    // @return .bIsValid is true if valid. If false, data is not valid.
    FInventoryValidationResult AddItem_Validate(const FItemSlot& InSlot);

    // Internal function for adding item to inventory.
    // @return false if failed
    bool AddItem_Internal(
        const FDataTableRowHandle& InSlotRowHandle,
        int32 DesignatedIndex,
        int32 MaxStackSize,
        int32 MaxSlotSize,
        int32& InOutRemainingQuantity,
        TArray<FItemSlot>& ItemArray
    );

    // DesignatedIndex < 0
    bool AddItem_AutoPlacement(
        const FDataTableRowHandle& InSlotRowHandle,
        int32 MaxStackSize,
        int32 MaxSlotSize,
        int32& InOutRemainingQuantity,
        TArray<FItemSlot>& ItemArray
    );

    // DesignatedIndex >= 0
    bool AddItem_ToDesignatedSlot(
        const FDataTableRowHandle& InSlotRowHandle,
        int32 DesignatedIndex,
        int32 MaxStackSize,
        int32& InOutRemainingQuantity,
        TArray<FItemSlot>& ItemArray
    );

    // Validate item action request, and get related data.
    // .ItemSlot points to inventory's slot which matches the request's slot.
    // @return .bIsValid is true if valid. If false, data is not valid.
    FInventoryValidationResult ValidateActionRequest(const FItemActionRequest& Request);

    // Remove item quantity from the slot.
    // Remove empty slot or update UI if needed.
    // @return Actually removed quantity.
    int32 RemoveItem_Internal(FInventoryValidationResult& InOutValidationResult, int32 ValidatedIndex, int32 Quantity);

    // @return Actually used quantity, -1 if failed.
    int32 UseItem_Internal(const FItemSlot& InSlot, FGameplayTag ItemType, int32 Quantity);

    // @return true if successfully equipped.
    bool UseItem_Equip(const FItemSlot& InSlot);

    // @return Actually used quantity, -1 if failed.
    int32 UseItem_Consume(const FItemSlot& InSlot, FGameplayTag ItemType, int32 Quantity);

    // @return Actually used quantity, -1 if failed.
    int32 ConsumeFood(const FConsumableData* ConsumableData, int32 Quantity);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    const TMap<FGameplayTag, FItemArray>& GetOwnedItems() const { return OwnedItems; }

private:
    UEquipmentComponent* GetEquipmentComponent();
    UStatsComponent* GetStatsComponent();

    ////////////////////////////////////////////////////////
    //        Variables - Inventory
    ////////////////////////////////////////////////////////
private:
    // {ItemCategory, Array<ItemSlot>}
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TMap<FGameplayTag, FItemArray> OwnedItems;

    // @check - can be added by purchase?
    // If changed, this should be applied to OwnedItems if bFixSlotSizeAndExposeEmptySlots == true.
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TMap<FGameplayTag, int32> MaxSlotSizes;

    ////////////////////////////////////////////////////////
    //        Variables - General
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    bool bFixSlotSizeAndExposeEmptySlots{true};

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    bool bAllowMultipleSlots{true};

    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int32 MaxAllowedSlotSize{1000};

    TWeakObjectPtr<UEquipmentComponent> CachedEquipmentComponent;
    TWeakObjectPtr<UStatsComponent>    CachedStatsComponent;
};