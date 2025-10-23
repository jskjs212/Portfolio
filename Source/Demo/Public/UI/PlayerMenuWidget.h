// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TabMenuWidget.h"
#include "PlayerMenuWidget.generated.h"

class UButton;
class UEquipmentPageWidget;
class UInventoryPageWidget;
class UItemSlotDragDropOp;
class UStatsPageWidget;
class USystemMenuPageWidget;
class UTabButton;
class UTextBlock;

/**
 * Call InitPlayerMenu() after creating this widget.
 */
UCLASS()
class DEMO_API UPlayerMenuWidget : public UTabMenuWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    // Handle before child widgets.
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    // Handle after child widgets.
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    void InitPlayerMenu();

    UFUNCTION()
    void HideMenu();

private:
    bool DropInventoryItem(const UItemSlotDragDropOp* InDragDropOp);

    bool DropEquipmentItem(const UItemSlotDragDropOp* InDragDropOp);

    virtual void SetFocusToWidget(UWidget* InWidget) override;

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    /* Stats */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> StatsTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UStatsPageWidget> StatsPageWidget;

    /* Inventory */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> InventoryTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UInventoryPageWidget> InventoryPageWidget;

    /* Equipment */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> EquipmentTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UEquipmentPageWidget> EquipmentPageWidget;

    /* System */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> SystemMenuTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USystemMenuPageWidget> SystemMenuPageWidget;

    /* Else */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CloseButton;
};