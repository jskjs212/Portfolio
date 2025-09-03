// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DemoHUDWidget.h"
#include "Items/Item.h"
#include "UI/ItemInfoWidget.h"

void UDemoHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // Validation
    if (!ItemInfoWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("UDemoHUDWidget - Failed to bind widgets."));
        return;
    }

    // Hide interact widgets
    ItemInfoWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UDemoHUDWidget::UpdateInteractWidgets(IInteractable* Interactable)
{
    AItem* Item = Cast<AItem>(Interactable);

    if (Item && Item->GetItemSlot().IsValid())
    {
        ItemInfoWidget->UpdateItemInfo(Item->GetItemSlot());
        ItemInfoWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemInfoWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}