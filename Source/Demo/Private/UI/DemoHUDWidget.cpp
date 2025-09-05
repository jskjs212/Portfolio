// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DemoHUDWidget.h"
#include "Interfaces/Interactable.h"
#include "Items/Item.h"
#include "UI/ActionKeyWidget.h"
#include "UI/InteractPromptWidget.h"
#include "UI/ItemInfoWidget.h"

void UDemoHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // Validation
    if (!ItemInfoWidget || !InteractPromptWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("UDemoHUDWidget - Failed to bind widgets."));
        return;
    }

    // Hide interact widgets
    ItemInfoWidget->SetVisibility(ESlateVisibility::Hidden);
    InteractPromptWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UDemoHUDWidget::UpdateInteractWidgets(IInteractable* Interactable)
{
    AItem* Item = Cast<AItem>(Interactable);

    if (Item && Item->GetItemSlot().IsValid())
    {
        ItemInfoWidget->UpdateItemInfo(Item->GetItemSlot());
        ItemInfoWidget->SetVisibility(ESlateVisibility::Visible);

        // hardcoded:
        InteractPromptWidget->SetActionText(FText::FromString(TEXT("Pick Up")));
        InteractPromptWidget->ActionKeyWidget->SetKeyText(FText::FromString(TEXT("E")));
        InteractPromptWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemInfoWidget->SetVisibility(ESlateVisibility::Hidden);
        InteractPromptWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}