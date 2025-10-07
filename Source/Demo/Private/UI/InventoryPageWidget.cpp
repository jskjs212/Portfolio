// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InventoryPageWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/InventoryComponent.h"
#include "Components/WrapBox.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/ItemTypes.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "PlayerController/DemoPlayerController.h"
#include "UI/ContextMenuWidget.h"
#include "UI/ItemActionDispatcher.h"
#include "UI/ItemInfoWidget.h"
#include "UI/ItemSlotWidget.h"
#include "UI/TabButton.h"

void UInventoryPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(WeaponTabButton && WeaponTabImage && WeaponWrapBox
        && ArmorTabButton && ArmorTabImage && ArmorWrapBox
        && ConsumableTabButton && ConsumableTabImage && ConsumableWrapBox
        && ItemInfoWidget && ItemPreviewBorder && ItemPreviewImage,
        TEXT("Failed to bind widgets."));

    if (!ItemSlotWidgetClass || !ContextMenuWidgetClass)
    {
        DemoLOG_CF(LogUI, Error, TEXT("WBP classes are not set."));
        return;
    }

    bUseTabButtonImages = true;

    // Setup tab entries
    // MUST sync with DemoGameplayTags::ItemCategories.
    TabEntries.Emplace(DemoGameplayTags::Item_Weapon, WeaponTabButton, WeaponTabImage, WeaponWrapBox);
    TabEntries.Emplace(DemoGameplayTags::Item_Armor, ArmorTabButton, ArmorTabImage, ArmorWrapBox);
    TabEntries.Emplace(DemoGameplayTags::Item_Consumable, ConsumableTabButton, ConsumableTabImage, ConsumableWrapBox);

    InitTabMenu();
    SetupContextMenu();
    HideItemInfo();
    BindToInventoryUpdates();
}

FReply UInventoryPageWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    static const TMap<FKey, int32> KeyToIndexMap{
        {EKeys::One, 0},
        {EKeys::Two, 1},
        {EKeys::Three, 2},
        {EKeys::Four, 3},
        {EKeys::Five, 4},
        {EKeys::Six, 5},
        {EKeys::Seven, 6},
        {EKeys::Eight, 7},
        {EKeys::Nine, 8}
    };

    // Key(1~9) -> select tab
    const int32* IndexPtr = KeyToIndexMap.Find(InKeyEvent.GetKey());
    if (IndexPtr)
    {
        if (*IndexPtr < TabEntries.Num())
        {
            SelectTab(*IndexPtr);
            return FReply::Handled();
        }
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UInventoryPageWidget::UpdateInventorySlotsUI()
{
    if (!IsVisible())
    {
        bPendingUpdateSlotsUI = true;
        return;
    }
    bPendingUpdateSlotsUI = false;

    const APawn* OwningPawn = GetOwningPlayerPawn();
    if (!OwningPawn)
    {
        return;
    }

    const UInventoryComponent* InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InventoryComponent)
    {
        return;
    }

    const TMap<FGameplayTag, FItemArray>& OwnedItems = InventoryComponent->GetOwnedItems();

    // For each item category
    for (FTabEntry& TabEntry : TabEntries)
    {
        UWrapBox* WrapBox = Cast<UWrapBox>(TabEntry.Widget);
        const TArray<FItemSlot>& ItemArray = OwnedItems[TabEntry.Tag].ItemArray;

        checkf(WrapBox, TEXT("Invalid inventory."));

        // No items (empty wrapbox)
        // ItemArray.Num() == 0 means bFixSlotSizeAndExposeEmptySlots == false.
        // @TODO - Show text? Set color?

        const int32 ItemNum = ItemArray.Num();
        const int32 ExistingNum = WrapBox->GetAllChildren().Num();
        const int32 NewSlotNum = FMath::Max(ItemNum, ExistingNum);

        for (int32 Index = 0; Index < NewSlotNum; ++Index)
        {
            UItemSlotWidget* SlotWidget = nullptr;

            // Add a new slot if needed
            if (Index >= ExistingNum && Index < ItemNum)
            {
                SlotWidget = CreateWidget<UItemSlotWidget>(this, ItemSlotWidgetClass);
                SlotWidget->SetIndex(Index);
                SlotWidget->SetSourceTag(DemoGameplayTags::UI_PlayerMenu_Inventory);
                SlotWidget->OnRightClicked.BindUObject(this, &ThisClass::HandleItemSlotRightClicked);
                SlotWidget->OnLeftDoubleClicked.BindUObject(this, &ThisClass::HandleItemSlotLeftDoubleClicked);
                SlotWidget->OnHovered.BindUObject(this, &ThisClass::ShowItemInfo);
                SlotWidget->OnUnhovered.BindUObject(this, &ThisClass::HideItemInfo);
                SlotWidget->OnDropped.BindUObject(this, &ThisClass::HandleItemSlotDropped);
                WrapBox->AddChildToWrapBox(SlotWidget);
                // ExistingNum += 1, but don't need to update.
            }
            // Or get existing slot
            else if (Index < ExistingNum)
            {
                SlotWidget = Cast<UItemSlotWidget>(WrapBox->GetChildAt(Index));
            }

            checkf(SlotWidget, TEXT("Undefined behavior."));

            // Update slot
            if (Index < ItemNum)
            {
                SlotWidget->SetItemSlot(ItemArray[Index]);
                SlotWidget->UpdateVisuals();
                SlotWidget->SetVisibility(ESlateVisibility::Visible);
            }
            // Hide unused slots
            else if (Index < ExistingNum)
            {
                SlotWidget->SetVisibility(ESlateVisibility::Collapsed);
            }
        }
    }
}

void UInventoryPageWidget::SetupContextMenu()
{
    TArray<FContextAction> Actions;
    Actions.Emplace(FText::FromString(TEXT("Use")), DemoGameplayTags::UI_Action_Item_Use);
    Actions.Emplace(FText::FromString(TEXT("Drop")), DemoGameplayTags::UI_Action_Item_Drop);
    Actions.Emplace(FText::FromString(TEXT("Cancel")), DemoGameplayTags::UI_Action_Item_Cancel);

    // Create context menu
    ContextMenuWidget = CreateWidget<UContextMenuWidget>(this, ContextMenuWidgetClass);
    ContextMenuWidget->InitActions(Actions);
    ContextMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
    ContextMenuWidget->AddToViewport(3);

    // Bind context menu buttons
    TArray<TObjectPtr<UTabButton>>& Buttons = ContextMenuWidget->GetActionButtons();
    for (TObjectPtr<UTabButton>& Button : Buttons)
    {
        Button->OnTabButtonClicked.BindUObject(this, &ThisClass::HandleContextMenuButtonClicked);
    }
}

void UInventoryPageWidget::BindToInventoryUpdates()
{
    if (APawn* OwningPawn = GetOwningPlayerPawn())
    {
        if (UInventoryComponent* InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>())
        {
            InventoryComponent->OnInventoryUpdated.AddUObject(this, &ThisClass::UpdateInventorySlotsUI);
            UpdateInventorySlotsUI();
        }
    }
}

UItemActionDispatcher* UInventoryPageWidget::GetItemActionDispatcher() const
{
    if (const ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
    {
        return DemoPlayerController->GetItemActionDispatcher();
    }
    return nullptr;
}

void UInventoryPageWidget::HandleContextMenuButtonClicked(FGameplayTag InTag)
{
    UItemActionDispatcher* ItemActionDispatcher = GetItemActionDispatcher();
    if (!ItemActionDispatcher)
    {
        return;
    }

    // Execute action
    if (InTag == DemoGameplayTags::UI_Action_Item_Use)
    {
        // @TODO - Get quantity from user input
        ContextMenuItemActionRequest.Quantity = 1;
        ItemActionDispatcher->RequestUseItem(ContextMenuItemActionRequest);
    }
    else if (InTag == DemoGameplayTags::UI_Action_Item_Drop)
    {
        // @TODO - Get quantity from user input
        ContextMenuItemActionRequest.Quantity = 1;
        ItemActionDispatcher->RequestDropItem(ContextMenuItemActionRequest);
    }
    else if (InTag == DemoGameplayTags::UI_Action_Item_Cancel)
    {
        // Do nothing
    }
    else
    {
        checkNoEntry();
    }

    ContextMenuWidget->HideContextMenu();
}

void UInventoryPageWidget::HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex)
{
    SetContextMenuData(InSlot, InIndex);
    ContextMenuWidget->ShowContextMenu();
}

void UInventoryPageWidget::HandleItemSlotLeftDoubleClicked(const FItemSlot& InSlot, int32 InIndex)
{
    UItemActionDispatcher* ItemActionDispatcher = GetItemActionDispatcher();
    if (!ItemActionDispatcher)
    {
        return;
    }

    // Use item (only one)
    FItemActionRequest Request;
    Request.Slot = InSlot;
    Request.DesignatedIndex = InIndex;
    Request.Quantity = 1;

    ItemActionDispatcher->RequestUseItem(Request);
}

void UInventoryPageWidget::HandleItemSlotDropped(const FItemSlot& SrcSlot, const int32 SrcIndex, const FItemSlot& DstSlot, const int32 DstIndex)
{
    // Validation
    if (!SrcSlot.IsValid() || SrcIndex == DstIndex || SrcSlot.bIsLocked || DstSlot.bIsLocked)
    {
        return;
    }

    UItemActionDispatcher* ItemActionDispatcher = GetItemActionDispatcher();
    if (!ItemActionDispatcher)
    {
        return;
    }

    FItemDataBase* SrcItemData = SrcSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryPageWidget::HandleItemSlotDropped"));
    if (!SrcItemData)
    {
        return;
    }

    // Case1: Empty or different item -> Swap
    if (!DstSlot.IsValid() || SrcSlot.RowHandle != DstSlot.RowHandle)
    {
        const FGameplayTag ItemCategory = DemoItemTypes::GetItemCategory(SrcItemData->ItemType);

        ItemActionDispatcher->RequestSwapItem(ItemCategory, SrcIndex, DstIndex);
    }
    // Case2: Same item -> Merge
    else
    {
        FItemActionRequest Request;
        Request.Slot = SrcSlot;
        Request.DesignatedIndex = DstIndex;
        Request.Quantity = SrcSlot.Quantity;

        const int32 Added = ItemActionDispatcher->RequestAddItem(Request);
        if (Added > 0)
        {
            Request.Slot = DstSlot;
            Request.DesignatedIndex = SrcIndex;
            Request.Quantity = Added;
            // @misc - Potential bug if fails.
            ItemActionDispatcher->RequestRemoveItem(Request);
        }
    }
}

void UInventoryPageWidget::ShowItemInfo(const FItemSlot& InSlot)
{
    const FItemDataBase* ItemData = ItemInfoWidget->UpdateItemInfo(InSlot);
    if (ItemData)
    {
        ItemInfoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
        ItemPreviewBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
        ItemPreviewImage->SetBrushFromTexture(ItemData->Icon);
        ItemPreviewImage->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UInventoryPageWidget::HideItemInfo()
{
    ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
    ItemPreviewBorder->SetVisibility(ESlateVisibility::Collapsed);
    ItemPreviewImage->SetVisibility(ESlateVisibility::Collapsed);
}