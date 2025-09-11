// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TabMenuWidget.h"
#include "Items/ItemTypes.h"
#include "InventoryPageWidget.generated.h"

class UBorder;
class UContextMenuWidget;
class UImage;
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
    // Update item slots UI from owner pawn's InventoryComponent (when visible)
    void UpdateItemSlotsUI();

    void ShowContextMenu(const FItemSlot& InSlot, int32 InIndex);

    bool IsPendingUpdate() const { return bPendingUpdateItemSlots; }

    FORCEINLINE void SetActionRequest(const FItemSlot& InSlot, int32 DesignatedIndex)
    {
        ItemActionRequest.Slot = InSlot;
        ItemActionRequest.DesignatedIndex = DesignatedIndex;
        ItemActionRequest.Quantity = 0;
    }

private:
    void SetupContextMenu();

    void BindToInventoryUpdates();

    // Just enough to invalidate
    //void ClearActionRequest() { ItemActionRequest.Slot = FItemSlot{}; }

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    UFUNCTION()
    void HandleContextMenuButtonClicked(FGameplayTag InTag);

    void HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex);

    void ShowItemInfo(const FItemSlot& InSlot);

    void HideItemInfo();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
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

private:
    UPROPERTY(EditDefaultsOnly, Category = "ItemSlot")
    TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "ItemSlot")
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass;

    TObjectPtr<UContextMenuWidget> ContextMenuWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
    bool bPendingUpdateItemSlots{false};

    FItemActionRequest ItemActionRequest;
};