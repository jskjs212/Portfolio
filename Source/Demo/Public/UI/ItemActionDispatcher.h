// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemoTypes/ItemTypes.h"
#include "ItemActionDispatcher.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(int32, FOnItemActionRequested, const FItemActionRequest& /* Request */);
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FOnSwapItemRequested, FGameplayTag /* ItemCategory */, int32 /* FirstIndex */, int32 /* SecondIndex */);
DECLARE_DELEGATE_RetVal_OneParam(bool, FOnUnequipItemRequested, FGameplayTag /* EquipmentType */);

/**
 * @TODO - Consider RequestItemAction(ActionTag, Request) instead of separate functions.
 */
UCLASS()
class DEMO_API UItemActionDispatcher : public UObject
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnItemActionRequested OnAddItemRequested;
    FOnItemActionRequested OnRemoveItemRequested;
    FOnItemActionRequested OnUseItemRequested;
    FOnItemActionRequested OnDropItemRequested;
    FOnSwapItemRequested OnSwapItemRequested;
    FOnUnequipItemRequested OnUnequipItemRequested;
    FOnUnequipItemRequested OnUnequipAndDropItemRequested;

    ////////////////////////////////////////////////////////
    //        Request functions
    ////////////////////////////////////////////////////////
public:
    // Add item to inventory.
    // @return Actually added quantity, -1 if failed.
    int32 RequestAddItem(const FItemActionRequest& Request) const
    {
        return OnAddItemRequested.IsBound() ? OnAddItemRequested.Execute(Request) : -1;
    }

    // Remove item from inventory.
    // @return Actually added quantity, -1 if failed.
    int32 RequestRemoveItem(const FItemActionRequest& Request) const
    {
        return OnRemoveItemRequested.IsBound() ? OnRemoveItemRequested.Execute(Request) : -1;
    }

    // Use item from inventory.
    // @return Actually used quantity, -1 if failed.
    int32 RequestUseItem(const FItemActionRequest& Request) const
    {
        return OnUseItemRequested.IsBound() ? OnUseItemRequested.Execute(Request) : -1;
    }

    // Drop item from inventory.
    // @return Actually dropped quantity, -1 if failed.
    int32 RequestDropItem(const FItemActionRequest& Request) const
    {
        return OnDropItemRequested.IsBound() ? OnDropItemRequested.Execute(Request) : -1;
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
        return OnUnequipItemRequested.IsBound() ? OnUnequipItemRequested.Execute(EquipmentType) : false;
    }

    // @return true if successfully unequipped and dropped
    bool RequestUnequipAndDropItem(FGameplayTag EquipmentType) const
    {
        return OnUnequipAndDropItemRequested.IsBound() ? OnUnequipAndDropItemRequested.Execute(EquipmentType) : false;
    }
};