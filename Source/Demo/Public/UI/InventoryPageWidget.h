// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TabMenuWidget.h"
#include "Items/ItemTypes.h"
#include "InventoryPageWidget.generated.h"

class ADemoPlayerController;
class UBorder;
class UContextMenuWidget;
class UImage;
class UItemActionDispatcher;
class UItemInfoWidget;
class UItemSlotWidget;
class UTagButton;
class UWrapBox;

/**
 * Inventory page widget in the player menu.
 * ItemSlotWidgets from inventory are stored in WrapBoxes categorized by each ItemType.
 * Their indexes are matched with indexes in InventoryComponent's Inventory array.
 */
UCLASS()
class DEMO_API UInventoryPageWidget : public UTabMenuWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    // Update UI of inventory item slots from owner pawn's InventoryComponent (when visible)
    void UpdateInventorySlotsUI();

    bool IsPendingUpdate() const { return bPendingUpdateSlotsUI; }

private:
    void SetupContextMenu();

    void BindToInventoryUpdates();

    void ShowItemInfo(const FItemSlot& InSlot);

    void HideItemInfo();

    void PlayErrorSound();

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    void HandleContextMenuButtonClicked(FGameplayTag InTag);

    void HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex);

    void HandleItemSlotLeftDoubleClicked(const FItemSlot& InSlot, int32 InIndex);

    void HandleItemSlotHovered(const FItemSlot& InSlot);

    void HandleItemSlotUnhovered();

    void HandleItemSlotDropped(const FItemSlot& SrcSlot, int32 SrcIndex, const FItemSlot& DstSlot, int32 DstIndex);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
private:
    ADemoPlayerController* GetDemoPlayerController();

    UItemActionDispatcher* GetItemActionDispatcher();

    FORCEINLINE void SetContextMenuData(const FItemSlot& InSlot, int32 DesignatedIndex)
    {
        ContextMenuItemActionRequest.Slot = InSlot;
        ContextMenuItemActionRequest.DesignatedIndex = DesignatedIndex;
        ContextMenuItemActionRequest.Quantity = 0;
    }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    /* Weapon */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagButton> WeaponTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> WeaponTabImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> WeaponWrapBox;

    /* Armor */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagButton> ArmorTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ArmorTabImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> ArmorWrapBox;

    /* Consumable */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagButton> ConsumableTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ConsumableTabImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> ConsumableWrapBox;

    /* ItemInfo */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemInfoWidget> ItemInfoWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> ItemPreviewBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemPreviewImage;

    /* ContextMenu */
    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UContextMenuWidget> ContextMenuWidget;

    /* Else */
    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    bool bPendingUpdateSlotsUI{true};

    FItemActionRequest ContextMenuItemActionRequest;

    TWeakObjectPtr<ADemoPlayerController> CachedDemoPlayerController;
};