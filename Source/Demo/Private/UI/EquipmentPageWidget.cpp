// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/EquipmentPageWidget.h"
#include "Components/EquipmentComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "Items/Item.h"
#include "PlayerController/DemoPlayerController.h"
#include "UI/ContextMenuWidget.h"
#include "UI/ItemActionDispatcher.h"
#include "UI/ItemInfoWidget.h"
#include "UI/ItemSlotWidget.h"
#include "UI/TabButton.h"

void UEquipmentPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(WeaponSlot && ShieldSlot && ItemInfoWidget, TEXT("Failed to bind widgets."));

    if (!ContextMenuWidgetClass)
    {
        DemoLOG_CF(LogUI, Error, TEXT("ContextMenuWidgetClass is not set."));
        return;
    }

    InitEquipmentSlots();
    InitContextMenu();
    HideItemInfo();
    BindToEquipmentUpdates();
}

void UEquipmentPageWidget::UpdateEquippedItemSlotUI(FGameplayTag EquipmentType, const FItemSlot& EquippedSlot)
{
    for (const FEquipmentSlotData& SlotData : EquipmentSlots)
    {
        if (SlotData.EquipmentType == EquipmentType)
        {
            SlotData.SlotWidget->SetItemSlot(EquippedSlot);
            SlotData.SlotWidget->UpdateVisuals();
            return;
        }
    }
    DemoLOG_CF(LogUI, Error, TEXT("No slot for EquipmentType %s."), *EquipmentType.ToString());
}

void UEquipmentPageWidget::UpdateUnequippedItemSlotUI(FGameplayTag EquipmentType, const FItemSlot& /* UnequippedSlot */)
{
    for (const FEquipmentSlotData& SlotData : EquipmentSlots)
    {
        if (SlotData.EquipmentType == EquipmentType)
        {
            SlotData.SlotWidget->SetItemSlot(FItemSlot{});
            SlotData.SlotWidget->UpdateVisuals();
            return;
        }
    }
    DemoLOG_CF(LogUI, Error, TEXT("No slot for EquipmentType %s."), *EquipmentType.ToString());
}

void UEquipmentPageWidget::UpdateAllEquipmentSlotsUI()
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

    const UEquipmentComponent* EquipmentComponent = OwningPawn->FindComponentByClass<UEquipmentComponent>();
    if (!EquipmentComponent)
    {
        return;
    }

    const TMap<FGameplayTag, TObjectPtr<AItem>>& EquippedItems = EquipmentComponent->GetAllEquippedItems();

    for (const FEquipmentSlotData& SlotData : EquipmentSlots)
    {
        const TObjectPtr<AItem> EquippedItem = EquippedItems[SlotData.EquipmentType];
        if (EquippedItem)
        {
            SlotData.SlotWidget->SetItemSlot(EquippedItem->GetItemSlot());
        }
        else // Unequipped
        {
            SlotData.SlotWidget->SetItemSlot(FItemSlot{});
        }
        SlotData.SlotWidget->UpdateVisuals();
    }
}

void UEquipmentPageWidget::InitEquipmentSlots()
{
    // Link slot widgets with equipment types
    EquipmentSlots.Emplace(FEquipmentSlotData{DemoGameplayTags::Item_Weapon, WeaponSlot});
    EquipmentSlots.Emplace(FEquipmentSlotData{DemoGameplayTags::Item_Armor_Shield, ShieldSlot});

    // Set index and bind events
    for (int32 Index = 0; Index < EquipmentSlots.Num(); ++Index)
    {
        UItemSlotWidget* SlotWidget = EquipmentSlots[Index].SlotWidget;
        SlotWidget->SetIndex(Index);
        SlotWidget->SetSourceTag(DemoGameplayTags::UI_PlayerMenu_Equipment);
        SlotWidget->OnRightClicked.BindUObject(this, &ThisClass::HandleItemSlotRightClicked);
        SlotWidget->OnLeftDoubleClicked.BindUObject(this, &ThisClass::HandleItemSlotLeftDoubleClicked);
        SlotWidget->OnHovered.BindUObject(this, &ThisClass::ShowItemInfo);
        SlotWidget->OnUnhovered.BindUObject(this, &ThisClass::HideItemInfo);
    }
}

void UEquipmentPageWidget::InitContextMenu()
{
    TArray<FContextAction> Actions;
    Actions.Emplace(FText::FromString(TEXT("Unequip")), DemoGameplayTags::UI_Action_Item_Unequip);
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

void UEquipmentPageWidget::BindToEquipmentUpdates()
{
    if (APawn* OwningPawn = GetOwningPlayerPawn())
    {
        if (UEquipmentComponent* EquipmentComponent = OwningPawn->FindComponentByClass<UEquipmentComponent>())
        {
            EquipmentComponent->OnEquipped.AddUObject(this, &ThisClass::UpdateEquippedItemSlotUI);
            EquipmentComponent->OnUnequipped.AddUObject(this, &ThisClass::UpdateUnequippedItemSlotUI);
        }
    }
}

UItemActionDispatcher* UEquipmentPageWidget::GetItemActionDispatcher() const
{
    if (const ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
    {
        return DemoPlayerController->GetItemActionDispatcher();
    }
    return nullptr;
}

void UEquipmentPageWidget::HandleContextMenuButtonClicked(FGameplayTag InTag)
{
    UItemActionDispatcher* ItemActionDispatcher = GetItemActionDispatcher();
    if (!ItemActionDispatcher)
    {
        return;
    }

    // Execute action
    if (InTag == DemoGameplayTags::UI_Action_Item_Unequip)
    {
        ItemActionDispatcher->RequestUnequipItem(ContextMenuEquipmentType);
    }
    else if (InTag == DemoGameplayTags::UI_Action_Item_Drop)
    {
        ItemActionDispatcher->RequestUnequipAndDropItem(ContextMenuEquipmentType);
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

void UEquipmentPageWidget::HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex)
{
    SetContextMenuData(InIndex);
    ContextMenuWidget->ShowContextMenu();
}

void UEquipmentPageWidget::HandleItemSlotLeftDoubleClicked(const FItemSlot& InSlot, int32 InIndex)
{
    UItemActionDispatcher* ItemActionDispatcher = GetItemActionDispatcher();
    if (!ItemActionDispatcher)
    {
        return;
    }

    const FGameplayTag EquipmentType = EquipmentSlots[InIndex].EquipmentType;
    bool bUnequipped = ItemActionDispatcher->RequestUnequipItem(EquipmentType);
    if (bUnequipped)
    {
        HideItemInfo();
    }
}

void UEquipmentPageWidget::ShowItemInfo(const FItemSlot& InSlot)
{
    const FItemDataBase* ItemData = ItemInfoWidget->UpdateItemInfo(InSlot);
    if (ItemData)
    {
        ItemInfoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UEquipmentPageWidget::HideItemInfo()
{
    ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
}