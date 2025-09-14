// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DemoHUDWidget.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Interfaces/Interactable.h"
#include "Items/Item.h"
#include "UI/ActionKeyWidget.h"
#include "UI/InteractPromptWidget.h"
#include "UI/ItemInfoWidget.h"
#include "UI/StatBarWidget.h"

void UDemoHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ItemInfoWidget && InteractPromptWidget
        && HealthBarWidget && StaminaBarWidget,
        TEXT("Failed to bind widgets."));

    // Hide interact widgets
    ItemInfoWidget->SetVisibility(ESlateVisibility::Hidden);
    InteractPromptWidget->SetVisibility(ESlateVisibility::Hidden);
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
    AItem* Item = Cast<AItem>(Interactable);

    if (Item && Item->GetItemSlot().IsValid())
    {
        ItemInfoWidget->UpdateItemInfo(Item->GetItemSlot());
        ItemInfoWidget->SetVisibility(ESlateVisibility::Visible);

        // @hardcoded - Action text and key
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