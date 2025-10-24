// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TabMenuWidget.h"
#include "DemoTypes/ItemTypes.h"
#include "InventoryPageWidget.generated.h"

class ADemoPlayerController;
class UBorder;
class UContextMenuWidget;
class UImage;
class UItemActionDispatcher;
class UItemInfoWidget;
class UItemSlotWidget;
class UTabButton;
class UWrapBox;

/**
 *
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

    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

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

    ADemoPlayerController* GetDemoPlayerController();

    UItemActionDispatcher* GetItemActionDispatcher();

    FORCEINLINE void SetContextMenuData(const FItemSlot& InSlot, int32 DesignatedIndex)
    {
        ContextMenuItemActionRequest.Slot = InSlot;
        ContextMenuItemActionRequest.DesignatedIndex = DesignatedIndex;
        ContextMenuItemActionRequest.Quantity = 0;
    }

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

    void ShowItemInfo(const FItemSlot& InSlot);

    void HideItemInfo();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    /* Weapon */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> WeaponTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> WeaponTabImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> WeaponWrapBox;

    /* Armor */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> ArmorTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ArmorTabImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> ArmorWrapBox;

    /* Consumable */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> ConsumableTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ConsumableTabImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> ConsumableWrapBox;

    /* Else */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemInfoWidget> ItemInfoWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> ItemPreviewBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemPreviewImage;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UContextMenuWidget> ContextMenuWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    bool bPendingUpdateSlotsUI{true};

    FItemActionRequest ContextMenuItemActionRequest;

    TWeakObjectPtr<ADemoPlayerController> CachedDemoPlayerController;
};