// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/TableRowBases.h"
#include "Items/ItemTypes.h"

DEFINE_LOG_CATEGORY(LogInventory);

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Inventory setup for fixed item categories.
    for (const FGameplayTag& ItemCategory : DemoItemTypes::ItemCategories)
    {
        OwnedItems.Add(ItemCategory, FItemArray{});
        MaxSlotSizes.Add(ItemCategory, 0);
    }
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    checkf(OwnedItems.Num() == DemoItemTypes::ItemCategories.Num(), TEXT("OwnedItems should have all ItemCategories."));
    checkf(MaxSlotSizes.Num() == DemoItemTypes::ItemCategories.Num(), TEXT("MaxSlotSizes should have all ItemCategories."));

    InitMaxSlots();
}

int32 UInventoryComponent::AddItem(FItemSlot& InSlot, int32 DesignatedIndex)
{
    const int32 OriginalQuantity = InSlot.Quantity;
    int32 RemainingQuantity = InSlot.Quantity;
    FInventoryValidatedData ValidatedData;

    // Validate and get data
    bool bValid = AddItem_Validate(InSlot, ValidatedData);
    if (!bValid)
    {
        return -1; // Log in AddItem_Validate()
    }

    // Add item
    bool bSuccess = AddItem_Internal(InSlot, DesignatedIndex, ValidatedData.ItemData->MaxStackSize, MaxSlotSizes[ValidatedData.ItemCategory], RemainingQuantity, ValidatedData.ItemArray);
    if (!bSuccess)
    {
        return -1; // Log in AddItem_Internal()
    }

    // Update InSlot's quantity
    const int32 Added = OriginalQuantity - RemainingQuantity;
    InSlot.Quantity = RemainingQuantity;

    // Update UI
    if (Added > 0)
    {
        // TODO: UI (broadcast?)
    }

    UE_LOG(LogInventory, Display, TEXT("Add item - %s, %d"), *ValidatedData.ItemData->Name.ToString(), Added);
    return Added;
}

int32 UInventoryComponent::RemoveItem(const FItemActionRequest& Request)
{
    // Validate and get data
    FInventoryValidatedData ValidatedData;
    bool bValid = ValidateActionRequest(Request, ValidatedData);
    if (!bValid)
    {
        return -1; // Log in ValidateActionRequest()
    }

    // Remove item
    const int32 ToRemove = FMath::Min(Request.Quantity, ValidatedData.ItemSlot->Quantity);
    ValidatedData.ItemSlot->Quantity -= ToRemove;

    // Remove empty slot if needed
    if (!bFixSlotSizeAndExposeEmptySlots && ValidatedData.ItemSlot->Quantity == 0)
    {
        ValidatedData.ItemArray->RemoveAt(Request.InventoryIndex);
    }

    // Update UI
    if (ToRemove > 0)
    {
        // TODO: UI (broadcast?)
    }

    UE_LOG(LogInventory, Display, TEXT("Remove item - %s, %d"), *ValidatedData.ItemData->Name.ToString(), ToRemove);
    return ToRemove;
}

void UInventoryComponent::InitMaxSlots()
{
    // TODO: Config file or data table? Determined by character class or something?
    // Initial max slot sizes.
    constexpr int32 WeaponMaxSlots = 10;
    constexpr int32 ArmorMaxSlots = 10;
    constexpr int32 ConsumableMaxSlots = 20;
    MaxSlotSizes[DemoGameplayTags::Item_Weapon] = WeaponMaxSlots;
    MaxSlotSizes[DemoGameplayTags::Item_Armor] = ArmorMaxSlots;
    MaxSlotSizes[DemoGameplayTags::Item_Consumable] = ConsumableMaxSlots;

    // Fill with empty slots if needed.
    if (bFixSlotSizeAndExposeEmptySlots)
    {
        for (TPair<FGameplayTag, FItemArray>& Pair : OwnedItems)
        {
            Pair.Value.ItemArray.SetNum(MaxSlotSizes[Pair.Key]);
        }
    }
}

bool UInventoryComponent::AddItem_Validate(const FItemSlot& InSlot, FInventoryValidatedData& OutData)
{
    OutData = FInventoryValidatedData{};

    if (!InSlot.IsValid())
    {
        UE_LOG(LogInventory, Warning, TEXT("AddItem() - Slot is not valid."));
        return false;
    }

    if (InSlot.bIsLocked)
    {
        UE_LOG(LogInventory, Warning, TEXT("AddItem() - Slot is locked."));
        return false;
    }

    // Get ItemData
    const FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryComponent::AddItem()"));
    if (!ItemData)
    {
        UE_LOG(LogInventory, Error, TEXT("AddItem() - ItemData is not found."));
        return false;
    }

    // Find ItemType & ItemArray
    // ex) Item.Weapon tag from Item.Weapon.Melee.OneHanded tag
    FGameplayTag ItemCategory = DemoItemTypes::GetItemCategory(ItemData->ItemType);
    TArray<FItemSlot>* ItemArray = nullptr;
    if (ItemCategory.IsValid())
    {
        ItemArray = &OwnedItems[ItemCategory].ItemArray;
    }
    if (!ItemArray)
    {
        UE_LOG(LogInventory, Error, TEXT("AddItem() - ItemType is not valid."));
        return false;
    }

    OutData.ItemCategory = ItemCategory;
    OutData.ItemData = ItemData;
    OutData.ItemArray = ItemArray;
    return true;
}

bool UInventoryComponent::AddItem_Internal(FItemSlot& InSlot, int32 DesignatedIndex, int32 MaxStackSize, int32 MaxSlotSize, int32& InOutRemainingQuantity, TArray<FItemSlot>*& ItemArray)
{
    if (DesignatedIndex < 0)
    {
        return AddItem_AutoPlacement(InSlot, MaxStackSize, MaxSlotSize, InOutRemainingQuantity, ItemArray);
    }
    else if (DesignatedIndex < ItemArray->Num())
    {
        return AddItem_ToDesignatedSlot(InSlot, DesignatedIndex, MaxStackSize, InOutRemainingQuantity, ItemArray);
    }
    else
    {
        UE_LOG(LogInventory, Error, TEXT("AddItem() - DesignatedIndex is out of range."));
        return false;
    }
}

bool UInventoryComponent::AddItem_AutoPlacement(FItemSlot& InSlot, int32 MaxStackSize, int32 MaxSlotSize, int32& InOutRemainingQuantity, TArray<FItemSlot>*& ItemArray)
{
    bool bSlotExists = false;

    // Top-up existing slots with same item
    for (FItemSlot& Slot : *ItemArray)
    {
        if (!IsInventorySlotEmpty(Slot) && Slot.RowHandle == InSlot.RowHandle)
        {
            // There should be 0 or 1 existing slot if multiple slots are not allowed.
            check(bAllowMultipleSlots || !bSlotExists);

            const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize - Slot.Quantity);
            bSlotExists = true;
            Slot.Quantity += ToAdd;
            InOutRemainingQuantity -= ToAdd;
        }
    }

    // Add into new slots
    int32 EmptySlotIndex = 0;
    while (InOutRemainingQuantity > 0)
    {
        // allow - continue
        // !allow && exists - break
        // !allow && !exists - continue
        if (!bAllowMultipleSlots && bSlotExists)
        {
            UE_LOG(LogInventory, Display, TEXT("AddItem() - Multiple slots are not allowed."));
            break;
        }

        // Add a new slot
        if (ItemArray->Num() < MaxSlotSize)
        {
            const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize);
            bSlotExists = true;
            ItemArray->Add(FItemSlot{InSlot.RowHandle, ToAdd});
            InOutRemainingQuantity -= ToAdd;
        }
        // Fill an empty slot
        else if (bFixSlotSizeAndExposeEmptySlots)
        {
            // Find an empty slot
            while (EmptySlotIndex < ItemArray->Num() && !IsInventorySlotEmpty((*ItemArray)[EmptySlotIndex]))
            {
                ++EmptySlotIndex;
            }

            if (EmptySlotIndex < ItemArray->Num())
            {
                const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize);
                bSlotExists = true;
                (*ItemArray)[EmptySlotIndex] = FItemSlot{InSlot.RowHandle, ToAdd};
                InOutRemainingQuantity -= ToAdd;
            }
            else // Out of empty slots
            {
                UE_LOG(LogInventory, Display, TEXT("AddItem() - Inventory is full."));
                break;
            }
        }
        else if (ItemArray->Num() == MaxSlotSize)
        {
            UE_LOG(LogInventory, Display, TEXT("AddItem() - Inventory is full."));
            break;
        }
        else
        {
            checkNoEntry();
            break;
        }
    } // end of while (InOutRemainingQuantity > 0)

    return true;
}

bool UInventoryComponent::AddItem_ToDesignatedSlot(FItemSlot& InSlot, int32 DesignatedIndex, int32 MaxStackSize, int32& InOutRemainingQuantity, TArray<FItemSlot>*& ItemArray)
{
    if (!bAllowMultipleSlots)
    {
        // Check existing slots.
        // Cache if planning to use large arrays.
        for (int32 Index = 0; const FItemSlot& Slot : *ItemArray)
        {
            if (DesignatedIndex != Index && !IsInventorySlotEmpty(Slot) && Slot.RowHandle == InSlot.RowHandle)
            {
                UE_LOG(LogInventory, Display, TEXT("AddItem() - Multiple slots are not allowed."));
                return false;
            }
            ++Index;
        }
    }

    // Empty or same item
    FItemSlot& Slot = (*ItemArray)[DesignatedIndex];
    if (IsInventorySlotEmpty(Slot) || Slot.RowHandle == InSlot.RowHandle)
    {
        const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize - Slot.Quantity);
        Slot.RowHandle = InSlot.RowHandle;
        Slot.Quantity += ToAdd;
        InOutRemainingQuantity -= ToAdd;
    }
    else // Different item -> cancel
    {
        UE_LOG(LogInventory, Display, TEXT("AddItem() - Different item exists at designated index."));
        return false;
    }

    return true;
}

bool UInventoryComponent::ValidateActionRequest(const FItemActionRequest& Request, FInventoryValidatedData& OutData)
{
    OutData = FInventoryValidatedData{};

    if (Request.Quantity <= 0)
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - Quantity is not valid."));
        return false;
    }

    const FItemDataBase* ItemData = Request.Slot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryComponent::ValidateActionRequest()"));
    if (!ItemData)
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - ItemData is not valid."));
        return false;
    }

    FGameplayTag ItemCategory = DemoItemTypes::GetItemCategory(ItemData->ItemType);

    // Category is validated in data table.
    TArray<FItemSlot>& ItemArray = OwnedItems[ItemCategory].ItemArray;
    if (Request.InventoryIndex < 0 || Request.InventoryIndex >= ItemArray.Num())
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - InventoryIndex is out of range."));
        return false;
    }

    FItemSlot& Slot = ItemArray[Request.InventoryIndex];
    if (IsInventorySlotEmpty(Slot) || Slot.RowHandle != Request.Slot.RowHandle)
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - Item at InventoryIndex is different."));
        return false;
    }

    if (Slot.bIsLocked)
    {
        UE_LOG(LogInventory, Warning, TEXT("ValidateActionRequest() - Slot is locked."));
        return false;
    }

    OutData.ItemCategory = ItemCategory;
    OutData.ItemSlot = &Slot;
    OutData.ItemData = ItemData;
    OutData.ItemArray = &ItemArray;
    return true;
}