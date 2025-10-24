// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DemoHUDWidget.h"
#include "Components/StatsComponent.h"
#include "Interfaces/Interactable.h"
#include "Items/Item.h"
#include "UI/ActionKeyWidget.h"
#include "UI/HUDHelpWidget.h"
#include "UI/InteractPromptWidget.h"
#include "UI/ItemInfoWidget.h"
#include "UI/StatBarWidget.h"

void UDemoHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ItemInfoWidget && InteractPromptWidget
        && HealthBarWidget && StaminaBarWidget && HelpWidget,
        TEXT("Failed to bind widgets."));

    // Hide interact widgets
    ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
    InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UDemoHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Init stat bars
    APawn* OwnerPawn = GetOwningPlayerPawn();
    HealthBarWidget->InitStatBar(OwnerPawn, UStatsComponent::HealthTag);
    StaminaBarWidget->InitStatBar(OwnerPawn, UStatsComponent::StaminaTag);
}

void UDemoHUDWidget::UpdateInteractWidgets(IInteractable* Interactable)
{
    const AItem* Item = Cast<AItem>(Interactable);

    if (Item && Item->GetItemSlot().IsValid())
    {
        ItemInfoWidget->UpdateItemInfo(Item->GetItemSlot());
        ItemInfoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

        // @hardcoded - Action text and key
        InteractPromptWidget->SetActionText(FText::FromString(TEXT("Pick Up")));
        InteractPromptWidget->ActionKeyWidget->SetKeyText(FText::FromString(TEXT("E")));
        InteractPromptWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    else
    {
        ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
        InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDemoHUDWidget::ToggleHelpText()
{
    HelpWidget->ToggleHelpText();
}