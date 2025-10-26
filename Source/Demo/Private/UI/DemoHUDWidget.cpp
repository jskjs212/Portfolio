// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DemoHUDWidget.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Interfaces/Interactable.h"
#include "Items/Item.h"
#include "UI/ActionKeyWidget.h"
#include "UI/AIStatusWidget.h"
#include "UI/HUDHelpWidget.h"
#include "UI/InteractPromptWidget.h"
#include "UI/ItemInfoWidget.h"
#include "UI/StatBarWidget.h"

void UDemoHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ItemInfoWidget && InteractPromptWidget
        && HealthBarWidget && StaminaBarWidget
        && BossAIStatusWidget && HelpWidget,
        TEXT("Failed to bind widgets."));

    // Hide interact widgets
    ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
    InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
    BossAIStatusWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UDemoHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Init stat bars
    APawn* OwnerPawn = GetOwningPlayerPawn();
    HealthBarWidget->BindToStatsComponent(OwnerPawn, DemoGameplayTags::Stat_Resource_Health);
    StaminaBarWidget->BindToStatsComponent(OwnerPawn, DemoGameplayTags::Stat_Resource_Stamina);
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

void UDemoHUDWidget::ShowBossAIStatus(AActor* BossActor)
{
    if (BossActor)
    {
        BossAIStatusWidget->BindToActor(BossActor);
        BossAIStatusWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    else
    {
        BossAIStatusWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDemoHUDWidget::ToggleHelpText()
{
    HelpWidget->ToggleHelpText();
}