// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/TableRowBases.h"
#include "Items/ItemTypes.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // TODO: Config file or data table?
    // These elements should not be changed at runtime.
    OwnedItems.Add(DemoGameplayTags::Item_Weapon, FItemArray{});
    OwnedItems.Add(DemoGameplayTags::Item_Armor, FItemArray{});
    OwnedItems.Add(DemoGameplayTags::Item_Consumable, FItemArray{});

    // TODO: Config file or data table?
    constexpr int32 WeaponMaxSlots = 10;
    constexpr int32 ArmorMaxSlots = 10;
    constexpr int32 ConsumableMaxSlots = 20;
    BaseMaxSlotSizes.Add(DemoGameplayTags::Item_Weapon, WeaponMaxSlots);
    BaseMaxSlotSizes.Add(DemoGameplayTags::Item_Armor, ArmorMaxSlots);
    BaseMaxSlotSizes.Add(DemoGameplayTags::Item_Consumable, ConsumableMaxSlots);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // Fill with empty slots
    if (bFixSlotSizeAndExposeEmptySlots)
    {
        for (TPair<FGameplayTag, FItemArray>& Pair : OwnedItems)
        {
            Pair.Value.ItemArray.SetNum(BaseMaxSlotSizes[Pair.Key]);
        }
    }
}

int32 UInventoryComponent::AddItem(FItemSlot& InSlot, int32 DesignatedIndex)
{
    int32 MaxStackSize;
    FName ItemName;
    FGameplayTag ItemType;
    TArray<FItemSlot>* ItemArray;

    // Validate and get data
    bool bValid = ValidateInInventorySlot(InSlot, MaxStackSize, ItemName, ItemType, ItemArray);
    if (!bValid)
    {
        return -1; // Log in ValidateInInventorySlot()
    }

    // Add item
    const int32 OriginalQuantity = InSlot.Quantity;
    int32 RemainingQuantity = InSlot.Quantity;
    bool bSuccess = AddItem_Internal(InSlot, DesignatedIndex, MaxStackSize, BaseMaxSlotSizes[ItemType], RemainingQuantity, ItemArray);
    if (!bSuccess)
    {
        return -1; // Log in AddItem_Internal()
    }

    // Update InSlot
    const int32 Added = OriginalQuantity - RemainingQuantity;
    InSlot.Quantity = RemainingQuantity;

    // Update UI
    if (Added > 0)
    {
        // TODO: UI (broadcast?)
    }

    UE_LOG(LogTemp, Display, TEXT("Add item - %s, %d"), *ItemName.ToString(), Added);
    return Added;
}

bool UInventoryComponent::ValidateInInventorySlot(const FItemSlot& InSlot, int32& OutMaxStackSize, FName& OutName, FGameplayTag& OutItemType, TArray<FItemSlot>*& OutItemArray)
{
    if (!InSlot.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::ValidateInInventorySlot() - Slot is not valid."));
        return false;
    }

    // Get ItemData
    const FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryComponent::AddItem()"));
    if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::ValidateInInventorySlot() - ItemData is not found."));
        return false;
    }

    // Find ItemType & ItemArray
    // ex) Find Item.Weapon tag from Item.Weapon.Melee.OneHanded tag
    OutItemType = FGameplayTag::EmptyTag;
    OutItemArray = nullptr;
    for (const TPair<FGameplayTag, FItemArray>& Pair : OwnedItems)
    {
        if (ItemData->ItemType.MatchesTag(Pair.Key))
        {
            OutItemType = Pair.Key;
            OutItemArray = &OwnedItems[OutItemType].ItemArray;
            break;
        }
    }
    if (!OutItemType.IsValid() || !OutItemArray)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::ValidateInInventorySlot() - ItemType is not valid."));
        return false;
    }

    OutMaxStackSize = ItemData->MaxStackSize;
    OutName = ItemData->Name;
    return true;
}

bool UInventoryComponent::AddItem_Internal(FItemSlot& InSlot, int32 DesignatedIndex, int32 MaxStackSize, int32 BaseMaxSlotSize, int32& InOutRemainingQuantity, TArray<FItemSlot>*& ItemArray)
{
    if (DesignatedIndex < 0)
    {
        // Top-up existing slots
        bool bSlotExists = false;
        for (FItemSlot& Slot : *ItemArray)
        {
            if (!IsInventorySlotEmpty(Slot) && Slot.RowHandle == InSlot.RowHandle)
            {
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
                break;
            }

            // Add a new slot
            if (ItemArray->Num() < BaseMaxSlotSize)
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
                else // No more empty slots
                {
                    break;
                }
            }
        } // end of while (InOutRemainingQuantity > 0)
    } // end of if (DesignatedIndex < 0)
    else if (DesignatedIndex < ItemArray->Num())
    {
        FItemSlot& Slot = (*ItemArray)[DesignatedIndex];

        // Empty or same item
        if (IsInventorySlotEmpty(Slot) || Slot.RowHandle == InSlot.RowHandle)
        {
            const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize - Slot.Quantity);
            Slot.RowHandle = InSlot.RowHandle;
            Slot.Quantity += ToAdd;
            InOutRemainingQuantity -= ToAdd;
        }
        else // Different item -> cancel
        {
            UE_LOG(LogTemp, Display, TEXT("UInventoryComponent::AddItem() - Different item exists at designated index."));
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::AddItem() - DesignatedIndex is out of range."));
        return false;
    }

    return true;
}
