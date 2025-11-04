// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/ItemTypes.h"
#include "ItemActionDispatcher.generated.h"

DECLARE_DELEGATE_RetVal_TwoParams(int32, FOnItemActionRequested, FGameplayTag /* InActionTag */, const FItemActionRequest& /* Request */);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FOnSwapItemRequested, FGameplayTag /* ItemCategory */, int32 /* FirstIndex */, int32 /* SecondIndex */);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FOnUnequipItemActionRequested, FGameplayTag /* InActionTag */, FGameplayTag /* EquipmentType */);

/**
 * Dispatcher for item actions (add, remove, use, drop, swap, unequip).
 * UI widgets can request item actions when player interacts with them.
 * Inventory and equipment systems bind to the delegates and handle the requests.
 */
UCLASS()
class DEMO_API UItemActionDispatcher : public UObject
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnItemActionRequested OnItemActionRequested;
    FOnSwapItemRequested OnSwapItemRequested;
    FOnUnequipItemActionRequested OnUnequipItemActionRequested;

    ////////////////////////////////////////////////////////
    //        Request functions
    ////////////////////////////////////////////////////////
public:
    // Add item to inventory.
    // @return Actually added quantity, -1 if failed.
    int32 RequestAddItem(const FItemActionRequest& Request) const
    {
        return OnItemActionRequested.IsBound() ? OnItemActionRequested.Execute(DemoGameplayTags::UI_Action_Item_Add, Request) : -1;
    }

    // Remove item from inventory.
    // @return Actually added quantity, -1 if failed.
    int32 RequestRemoveItem(const FItemActionRequest& Request) const
    {
        return OnItemActionRequested.IsBound() ? OnItemActionRequested.Execute(DemoGameplayTags::UI_Action_Item_Remove, Request) : -1;
    }

    // Use item from inventory.
    // @return Actually used quantity, -1 if failed.
    int32 RequestUseItem(const FItemActionRequest& Request) const
    {
        return OnItemActionRequested.IsBound() ? OnItemActionRequested.Execute(DemoGameplayTags::UI_Action_Item_Use, Request) : -1;
    }

    // Drop item from inventory.
    // @return Actually dropped quantity, -1 if failed.
    int32 RequestDropItem(const FItemActionRequest& Request) const
    {
        return OnItemActionRequested.IsBound() ? OnItemActionRequested.Execute(DemoGameplayTags::UI_Action_Item_Drop, Request) : -1;
    }

    // Swap item between two inventory slots.
    // @return true if succeeded.
    bool RequestSwapItem(FGameplayTag ItemCategory, int32 FirstIndex, int32 SecondIndex) const
    {
        return OnSwapItemRequested.IsBound() ? OnSwapItemRequested.Execute(ItemCategory, FirstIndex, SecondIndex) : false;
    }

    // @return true if successfully unequipped
    bool RequestUnequipItem(FGameplayTag EquipmentType) const
    {
        return OnUnequipItemActionRequested.IsBound() ? OnUnequipItemActionRequested.Execute(DemoGameplayTags::UI_Action_Item_Unequip, EquipmentType) : false;
    }

    // @return true if successfully unequipped and dropped
    bool RequestUnequipAndDropItem(FGameplayTag EquipmentType) const
    {
        return OnUnequipItemActionRequested.IsBound() ? OnUnequipItemActionRequested.Execute(DemoGameplayTags::UI_Action_Item_UnequipAndDrop, EquipmentType) : false;
    }
};