// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "EquipmentPageWidget.generated.h"

class UContextMenuWidget;
class UItemActionDispatcher;
class UItemInfoWidget;
class UItemSlotWidget;

struct FEquipmentSlotData
{
    FGameplayTag EquipmentType;

    TObjectPtr<UItemSlotWidget> SlotWidget;
};

UCLASS()
class DEMO_API UEquipmentPageWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    void UpdateEquippedItemSlotUI(FGameplayTag EquipmentType, const FItemSlot& EquippedSlot);
    void UpdateUnequippedItemSlotUI(FGameplayTag EquipmentType, const FItemSlot& /* UnequippedSlot */);

    void UpdateAllEquipmentSlotsUI();

    bool IsPendingUpdate() const { return bPendingUpdateSlotsUI; }

    FGameplayTag GetEquipmentTypeAtIndex(int32 InIndex) const
    {
        return EquipmentSlots.IsValidIndex(InIndex) ? EquipmentSlots[InIndex].EquipmentType : FGameplayTag::EmptyTag;
    }

private:
    void InitEquipmentSlots();

    void InitContextMenu();

    void BindToEquipmentUpdates();

    UItemActionDispatcher* GetItemActionDispatcher() const;

    // Only for RMB context menu = valid index.
    FORCEINLINE void SetContextMenuData(int32 InIndex)
    {
        ContextMenuEquipmentType = EquipmentSlots[InIndex].EquipmentType;
    }

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    void HandleContextMenuButtonClicked(FGameplayTag InTag);

    void HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex);

    void HandleItemSlotLeftDoubleClicked(const FItemSlot& InSlot, int32 InIndex);

    void ShowItemInfo(const FItemSlot& InSlot);

    void HideItemInfo();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
protected:
    /* Equipment slots */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemSlotWidget> WeaponSlot;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemSlotWidget> ShieldSlot;

    /* Else */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemInfoWidget> ItemInfoWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UContextMenuWidget> ContextMenuWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    bool bPendingUpdateSlotsUI{true};

    FGameplayTag ContextMenuEquipmentType;

    TArray<FEquipmentSlotData> EquipmentSlots;
};