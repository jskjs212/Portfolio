// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/EquipmentPageWidget.h"
#include "Components/EquipmentComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Items/Item.h"
#include "PlayerController/DemoPlayerController.h"
#include "UI/ContextMenuWidget.h"
#include "UI/ItemActionDispatcher.h"
#include "UI/ItemSlotWidget.h"
#include "UI/TabButton.h"

void UEquipmentPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(WeaponSlot && ShieldSlot, TEXT("Failed to bind widgets."));

    if (!ContextMenuWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("UEquipmentPageWidget::NativeOnInitialized - WBP classes are not set."));
        return;
    }

    InitEquipmentSlots();
    InitContextMenu();
    BindToEquipmentUpdates();
}

void UEquipmentPageWidget::UpdateEquipmentSlotsUI(FGameplayTag EquipmentType)
{
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

    for (FEquipmentSlotData& SlotData : EquipmentSlots)
    {
        if (SlotData.EquipmentType == EquipmentType)
        {
            const TObjectPtr<AItem> EquippedItem = EquippedItems[SlotData.EquipmentType];
            if (EquippedItem)
            {
                SlotData.SlotWidget->SetItemSlot(EquippedItem->GetItemSlot());
            }
            else
            {
                SlotData.SlotWidget->SetItemSlot(FItemSlot{});
            }
            SlotData.SlotWidget->UpdateVisuals();
        }
    }
}

void UEquipmentPageWidget::InitEquipmentSlots()
{
    // @TODO - bind to slot events
    int32 Index = 0;

    WeaponSlot->SetIndex(Index++);
    WeaponSlot->OnRightClicked.BindUObject(this, &ThisClass::HandleItemSlotRightClicked);
    EquipmentSlots.Emplace(FEquipmentSlotData{DemoGameplayTags::Item_Weapon, WeaponSlot});

    ShieldSlot->SetIndex(Index++);
    ShieldSlot->OnRightClicked.BindUObject(this, &ThisClass::HandleItemSlotRightClicked);
    EquipmentSlots.Emplace(FEquipmentSlotData{DemoGameplayTags::Item_Armor_Shield, ShieldSlot});
}

void UEquipmentPageWidget::InitContextMenu()
{
    TArray<FContextAction> Actions;
    Actions.Emplace(FText::FromString(TEXT("Unequip")), DemoGameplayTags::UI_Action_Item_Unequip);
    Actions.Emplace(FText::FromString(TEXT("Drop")), DemoGameplayTags::UI_Action_Item_Drop);
    Actions.Emplace(FText::FromString(TEXT("Cancel")), DemoGameplayTags::UI_Action_Item_Cancel);

    // Create context menu
    ContextMenuWidget = CreateWidget<UContextMenuWidget>(this, ContextMenuWidgetClass);
    ContextMenuWidget->SetupActions(Actions);
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
            EquipmentComponent->OnEquipmentChanged.AddUObject(this, &ThisClass::UpdateEquipmentSlotsUI);
            //UpdateEquipmentSlotsUI();
        }
    }
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
        const int32 Index = ContextMenuItemActionRequest.DesignatedIndex;
        const FGameplayTag EquipmentType = EquipmentSlots[Index].EquipmentType;
        ItemActionDispatcher->RequestUnequipItem(EquipmentType);
    }
    else if (InTag == DemoGameplayTags::UI_Action_Item_Drop)
    {
        // @TODO - Drop
        // 1) Unequip item if equipped
        // 2) Drop item from inventory
        // Problem: Should be able to drop even the inventory is full
        // Solution 1: Add a new unequip & drop in EquipmentComponent?
        // Solution 2: Just add a bool bAllowDropWhenInventoryFull in DropItem()?
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentPageWidget::HandleContextMenuButtonClicked - Drop not implemented yet."));
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

UItemActionDispatcher* UEquipmentPageWidget::GetItemActionDispatcher() const
{
    if (const ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
    {
        return DemoPlayerController->GetItemActionDispatcher();
    }
    return nullptr;
}

void UEquipmentPageWidget::HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex)
{
    SetActionRequest(InSlot, InIndex);
    ContextMenuWidget->ShowContextMenu();
}