// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerMenuWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "PlayerController/DemoPlayerController.h"
#include "UI/EquipmentPageWidget.h"
#include "UI/InventoryPageWidget.h"
#include "UI/ItemActionDispatcher.h"
#include "UI/ItemSlotDragDropOp.h"
#include "UI/SettingsPageWidget.h"
#include "UI/StatsPageWidget.h"
#include "UI/SystemMenuPageWidget.h"

void UPlayerMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(StatsTabButton && StatsPageWidget
        && InventoryTabButton && InventoryPageWidget
        && EquipmentTabButton && EquipmentPageWidget
        && SettingsTabButton && SettingsPageWidget
        && SystemMenuTabButton && SystemMenuPageWidget
        && CloseButton,
        TEXT("Failed to bind widgets."));

    bUseTabButtonImages = false;

    // Setup tab entries
    TabEntries.Emplace(DemoGameplayTags::UI_PlayerMenu_Stats, StatsTabButton, nullptr, StatsPageWidget);
    TabEntries.Emplace(DemoGameplayTags::UI_PlayerMenu_Inventory, InventoryTabButton, nullptr, InventoryPageWidget);
    TabEntries.Emplace(DemoGameplayTags::UI_PlayerMenu_Equipment, EquipmentTabButton, nullptr, EquipmentPageWidget);
    TabEntries.Emplace(DemoGameplayTags::UI_PlayerMenu_Settings, SettingsTabButton, nullptr, SettingsPageWidget);
    TabEntries.Emplace(DemoGameplayTags::UI_PlayerMenu_SystemMenu, SystemMenuTabButton, nullptr, SystemMenuPageWidget);
    InitTabMenu();

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
    if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
    {
        DemoPlayerController->SetCursorState(ECursorState::Default);
    }

    const UItemSlotDragDropOp* DragDropOp = Cast<UItemSlotDragDropOp>(InOperation);
    if (DragDropOp)
    {
        if (DragDropOp->GetSourceTag() == DemoGameplayTags::UI_PlayerMenu_Inventory)
        {
            return DropInventoryItem(DragDropOp);
        }
        else if (DragDropOp->GetSourceTag() == DemoGameplayTags::UI_PlayerMenu_Equipment)
        {
            return DropEquipmentItem(DragDropOp);
        }
        else
        {
            // Should handle other source tags if any. (e.g. Stash, Vendor, etc.)
            checkNoEntry();
        }
    }

    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UPlayerMenuWidget::InitPlayerMenu()
{
    StatsPageWidget->UpdateAllStats();
    SettingsPageWidget->SyncUIWithUserSettings();
}

void UPlayerMenuWidget::HideMenu()
{
    if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
    {
        CancelDragDrop();
        DemoPlayerController->ShowPlayerMenu(false);
    }
}

bool UPlayerMenuWidget::DropInventoryItem(const UItemSlotDragDropOp* InDragDropOp)
{
    if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
    {
        if (UItemActionDispatcher* ItemActionDispatcher = DemoPlayerController->GetItemActionDispatcher())
        {
            FItemActionRequest Request;
            Request.Slot = InDragDropOp->GetItemSlot();
            Request.DesignatedIndex = InDragDropOp->GetIndex();
            Request.Quantity = InDragDropOp->GetItemSlot().Quantity;

            return ItemActionDispatcher->RequestDropItem(Request) > 0;
        }
    }
    return false;
}

bool UPlayerMenuWidget::DropEquipmentItem(const UItemSlotDragDropOp* InDragDropOp)
{
    if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
    {
        if (UItemActionDispatcher* ItemActionDispatcher = DemoPlayerController->GetItemActionDispatcher())
        {
            FGameplayTag EquipmentType = EquipmentPageWidget->GetEquipmentTypeAtIndex(InDragDropOp->GetIndex());

            return ItemActionDispatcher->RequestUnequipAndDropItem(EquipmentType);
        }
    }
    return false;
}

void UPlayerMenuWidget::SetFocusToWidget(UWidget* InWidget)
{
    Super::SetFocusToWidget(InWidget);

    if (UInventoryPageWidget* InventoryPage = Cast<UInventoryPageWidget>(InWidget))
    {
        if (InventoryPage->IsPendingUpdate())
        {
            InventoryPage->UpdateInventorySlotsUI();
        }
    }
    else if (UEquipmentPageWidget* EquipmentPage = Cast<UEquipmentPageWidget>(InWidget))
    {
        if (EquipmentPage->IsPendingUpdate())
        {
            EquipmentPage->UpdateAllEquipmentSlotsUI();
        }
    }
}