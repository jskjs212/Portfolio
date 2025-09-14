// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerMenuWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "PlayerController/DemoPlayerController.h"
#include "UI/InventoryPageWidget.h"
#include "UI/ItemActionDispatcher.h"
#include "UI/ItemSlotDragDropOp.h"
#include "UI/StatsPageWidget.h"

void UPlayerMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(StatsTabButton && StatsPageWidget
        && InventoryTabButton && InventoryPageWidget
        && CloseButton,
        TEXT("Failed to bind widgets."));

    // Setup tab entries
    TabEntries.Add(FTabEntry{DemoGameplayTags::UI_PlayerMenu_Stats, StatsTabButton, nullptr, StatsPageWidget});
    TabEntries.Add(FTabEntry{DemoGameplayTags::UI_PlayerMenu_Inventory, InventoryTabButton, nullptr, InventoryPageWidget});

    InitMenu();

    CloseButton->OnClicked.AddDynamic(this, &UPlayerMenuWidget::HideMenu);
}

FReply UPlayerMenuWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Key(Tab) -> close
    if (InKeyEvent.GetKey() == EKeys::Tab)
    {
        HideMenu();
        return FReply::Handled();
    }

    // Key(Q) -> left tab
    if (InKeyEvent.GetKey() == EKeys::Q)
    {
        const int32 CurrentIndex = PageSwitcher->GetActiveWidgetIndex();
        const int32 NewIndex = CurrentIndex == 0 ? TabEntries.Num() - 1 : CurrentIndex - 1;
        SelectTab(NewIndex);
        return FReply::Handled();
    }

    // Key(E) -> right tab
    if (InKeyEvent.GetKey() == EKeys::E)
    {
        const int32 CurrentIndex = PageSwitcher->GetActiveWidgetIndex();
        const int32 NewIndex = CurrentIndex == TabEntries.Num() - 1 ? 0 : CurrentIndex + 1;
        SelectTab(NewIndex);
        return FReply::Handled();
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UPlayerMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Key(Escape) -> close
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        HideMenu();
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UPlayerMenuWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Any button -> cancel drag & drop
    if (UWidgetBlueprintLibrary::IsDragDropping())
    {
        UWidgetBlueprintLibrary::CancelDragDrop();
        return FReply::Handled();
    }

    return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

bool UPlayerMenuWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // Item slot dragged & dropped: Only drop item for now
    // @check - Dragged from other page i.e. unequip, buy, move from stash, etc.
    UItemSlotDragDropOp* DragDropOp = Cast<UItemSlotDragDropOp>(InOperation);
    if (DragDropOp)
    {
        FItemActionRequest Request;
        Request.Slot = DragDropOp->GetItemSlot();
        Request.DesignatedIndex = DragDropOp->GetIndex();
        Request.Quantity = DragDropOp->GetItemSlot().Quantity;

        if (ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
        {
            if (UItemActionDispatcher* ItemActionDispatcher = DemoPlayerController->GetItemActionDispatcher())
            {
                ItemActionDispatcher->RequestDropItem(Request);
                return true;
            }
        }
    }

    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UPlayerMenuWidget::HideMenu()
{
    if (ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
    {
        CancelDragDrop();
        DemoPlayerController->ShowPlayerMenu(false);
    }
}

void UPlayerMenuWidget::SetFocusToWidget(UWidget* InWidget)
{
    Super::SetFocusToWidget(InWidget);

    if (UInventoryPageWidget* InventoryPage = Cast<UInventoryPageWidget>(InWidget))
    {
        if (InventoryPage->IsPendingUpdate())
        {
            InventoryPage->UpdateItemSlotsUI();
        }
    }
}