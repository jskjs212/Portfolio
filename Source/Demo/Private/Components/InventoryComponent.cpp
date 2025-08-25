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
    if (!InSlot.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItem() - Slot is not valid."));
        return -1;
    }

    // Get ItemData
    const FItemDataBase* ItemData = InSlot.ItemID.GetRow<FItemDataBase>(TEXT("UInventoryComponent::AddItem()"));
    if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::AddItem() - ItemData is not found."));
        return -1;
    }

    // Find ItemType & ItemArray
    FGameplayTag ItemType{};
    TArray<FItemSlot>* ItemArray{nullptr};
    for (const TPair<FGameplayTag, FItemArray>& Pair : OwnedItems)
    {
        // ex) Find Item.Weapon for Item.Weapon.Melee.OneHanded
        if (ItemData->ItemType.MatchesTag(Pair.Key))
        {
            ItemType = Pair.Key;
            ItemArray = &OwnedItems[ItemType].ItemArray;
            break;
        }
    }
    if (!ItemType.IsValid() || !ItemArray)
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::AddItem() - ItemType is not valid."));
        return -1;
    }

    // Add item
    const int32 MaxStackSize = ItemData->MaxStackSize;
    int32 RemainingQuantity = InSlot.Quantity;
    if (DesignatedIndex < 0)
    {
        // Top-up existing slots
        bool bSlotExists = false;
        for (FItemSlot& Slot : *ItemArray)
        {
            if (!IsInventorySlotEmpty(Slot) && Slot.ItemID == InSlot.ItemID)
            {
                const int32 ToAdd = FMath::Min(RemainingQuantity, MaxStackSize - Slot.Quantity);
                bSlotExists = true;
                Slot.Quantity += ToAdd;
                RemainingQuantity -= ToAdd;
            }
        }

        // Add into new slots
        int32 EmptySlotIndex = 0;
        while (RemainingQuantity > 0)
        {
            // allow - continue
            // !allow && exists - break
            // !allow && !exists - continue
            if (!bAllowMultipleSlots && bSlotExists)
            {
                break;
            }

            // Add a new slot
            if (ItemArray->Num() < BaseMaxSlotSizes[ItemType])
            {
                const int32 ToAdd = FMath::Min(RemainingQuantity, MaxStackSize);
                bSlotExists = true;
                ItemArray->Add(FItemSlot{InSlot.ItemID, ToAdd});
                RemainingQuantity -= ToAdd;
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
                    const int32 ToAdd = FMath::Min(RemainingQuantity, MaxStackSize);
                    bSlotExists = true;
                    (*ItemArray)[EmptySlotIndex] = FItemSlot{InSlot.ItemID, ToAdd};
                    RemainingQuantity -= ToAdd;
                }
                else // Slots are full
                {
                    break;
                }
            }
        } // end of while (RemainingQuantity > 0)
    } // end of if (DesignatedIndex < 0)
    else if (DesignatedIndex < ItemArray->Num())
    {
        FItemSlot& Slot = (*ItemArray)[DesignatedIndex];

        // Different item -> cancel
        if (!IsInventorySlotEmpty(Slot) && Slot.ItemID != InSlot.ItemID)
        {
            return 0;
        }
        else  // Empty or same item
        {
            const int32 ToAdd = FMath::Min(RemainingQuantity, MaxStackSize - Slot.Quantity);
            Slot.ItemID = InSlot.ItemID;
            Slot.Quantity += ToAdd;
            RemainingQuantity -= ToAdd;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::AddItem() - DesignatedIndex is out of range."));
        return -1;
    }

    const int32 Added = InSlot.Quantity - RemainingQuantity;
    InSlot.Quantity = RemainingQuantity;

    // Update UI
    if (Added > 0)
    {
        // TODO: UI (broadcast?)
    }

    UE_LOG(LogTemp, Display, TEXT("Add item - %s, %d"), *ItemData->Name.ToString(), Added);
    return Added;
}