// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InventoryPageWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/InventoryComponent.h"
#include "Components/WrapBox.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "Items/ItemTypes.h"
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
        UE_LOG(LogTemp, Error, TEXT("UInventoryPageWidget::NativeOnInitialized - WBP class are not set."));
        return;
    }

    bUseTabButtonImages = true;

    // Setup tab entries
    // MUST sync with DemoGameplayTags::ItemCategories.
    TabEntries.Add(FTabEntry{DemoGameplayTags::Item_Weapon, WeaponTabButton, WeaponTabImage, WeaponWrapBox});
    TabEntries.Add(FTabEntry{DemoGameplayTags::Item_Armor, ArmorTabButton, ArmorTabImage, ArmorWrapBox});
    TabEntries.Add(FTabEntry{DemoGameplayTags::Item_Consumable, ConsumableTabButton, ConsumableTabImage, ConsumableWrapBox});

    InitMenu();
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

void UInventoryPageWidget::UpdateItemSlotsUI()
{
    if (!IsVisible())
    {
        bPendingUpdateItemSlots = true;
        return;
    }
    bPendingUpdateItemSlots = false;

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
        // TODO: Show text? Set color?

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
                SlotWidget->RightClicked.BindUObject(this, &ThisClass::HandleItemSlotRightClicked);
                SlotWidget->OnHovered.BindUObject(this, &ThisClass::ShowItemInfo);
                SlotWidget->OnUnhovered.BindUObject(this, &ThisClass::HideItemInfo);
                WrapBox->AddChildToWrapBox(SlotWidget);
                // ExistingNum += 1, but don't need to update.
            }
            // Or update existing slot
            else if (Index < ExistingNum)
            {
                SlotWidget = Cast<UItemSlotWidget>(WrapBox->GetChildAt(Index));
            }

            checkf(SlotWidget, TEXT("Undefined behavior."));

            // Update slot
            if (Index < ItemNum)
            {
                SlotWidget->UpdateItemSlot(ItemArray[Index], Index);
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

void UInventoryPageWidget::ShowContextMenu(const FItemSlot& InSlot, int32 InIndex)
{
    SetActionRequest(InSlot, InIndex);
    ContextMenuWidget->ShowContextMenu();
}

void UInventoryPageWidget::SetupContextMenu()
{
    TArray<FContextAction> Actions;
    Actions.Add(FContextAction{FText::FromString(TEXT("Use")), DemoGameplayTags::UI_Action_Item_Use});
    Actions.Add(FContextAction{FText::FromString(TEXT("Drop")), DemoGameplayTags::UI_Action_Item_Drop});
    Actions.Add(FContextAction{FText::FromString(TEXT("Cancel")), DemoGameplayTags::UI_Action_Item_Cancel});

    // Create context menu
    ContextMenuWidget = CreateWidget<UContextMenuWidget>(this, ContextMenuWidgetClass);
    checkf(ContextMenuWidget, TEXT("Failed to create ContextMenuWidget."));
    ContextMenuWidget->SetupActions(Actions);
    ContextMenuWidget->SetVisibility(ESlateVisibility::Hidden);
    ContextMenuWidget->AddToViewport(3);

    // Bind context menu buttons
    TArray<TObjectPtr<UTabButton>>& Buttons = ContextMenuWidget->GetActionButtons();
    Buttons[0]->OnTabButtonClicked.BindUObject(this, &ThisClass::HandleContextMenuButtonClicked);
    Buttons[1]->OnTabButtonClicked.BindUObject(this, &ThisClass::HandleContextMenuButtonClicked);
    Buttons[2]->OnTabButtonClicked.BindUObject(this, &ThisClass::HandleContextMenuButtonClicked);
}

void UInventoryPageWidget::BindToInventoryUpdates()
{
    if (APawn* OwningPawn = GetOwningPlayerPawn())
    {
        if (UInventoryComponent* InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>())
        {
            InventoryComponent->OnInventoryUpdated.AddUObject(this, &ThisClass::UpdateItemSlotsUI);
            UpdateItemSlotsUI();
        }
    }
}

void UInventoryPageWidget::HandleContextMenuButtonClicked(FGameplayTag InTag)
{
    // Validation
    ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>();
    if (!DemoPlayerController)
    {
        return;
    }

    UItemActionDispatcher* ItemActionDispatcher = DemoPlayerController->GetItemActionDispatcher();
    if (!ItemActionDispatcher)
    {
        return;
    }

    // Execute action
    if (InTag == DemoGameplayTags::UI_Action_Item_Use)
    {
        // TODO: Get quantity from user input
        ItemActionRequest.Quantity = 1;
        ItemActionDispatcher->RequestUseItem(ItemActionRequest);
    }
    else if (InTag == DemoGameplayTags::UI_Action_Item_Drop)
    {
        // TODO: Get quantity from user input
        ItemActionRequest.Quantity = 1;
        ItemActionDispatcher->RequestDropItem(ItemActionRequest);
    }
    else if (InTag == DemoGameplayTags::UI_Action_Item_Cancel)
    {
        // Do nothing
    }

    ContextMenuWidget->HideContextMenu();
}

void UInventoryPageWidget::HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex)
{
    ShowContextMenu(InSlot, InIndex);
}

void UInventoryPageWidget::ShowItemInfo(const FItemSlot& InSlot)
{
    const FItemDataBase* ItemData = ItemInfoWidget->UpdateItemInfo(InSlot);
    if (ItemData)
    {
        ItemInfoWidget->SetVisibility(ESlateVisibility::Visible);
        ItemPreviewBorder->SetVisibility(ESlateVisibility::Visible);
        ItemPreviewImage->SetBrushFromTexture(ItemData->Icon);
        ItemPreviewImage->SetVisibility(ESlateVisibility::Visible);
    }
}

void UInventoryPageWidget::HideItemInfo()
{
    ItemInfoWidget->SetVisibility(ESlateVisibility::Hidden);
    ItemPreviewBorder->SetVisibility(ESlateVisibility::Hidden);
    ItemPreviewImage->SetVisibility(ESlateVisibility::Hidden);
}