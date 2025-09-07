// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerMenuWidget.h"
#include "Components/Button.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "PlayerController/DemoPlayerController.h"
#include "UI/InventoryPageWidget.h"
#include "UI/StatsPageWidget.h"

void UPlayerMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(StatsTabButton && StatsPageWidget &&
        InventoryTabButton && InventoryPageWidget &&
        CloseButton,
        TEXT("Failed to bind widgets."));

    // Setup tab entries
    TabEntries.Add(FTabEntry{DemoGameplayTags::UI_PlayerMenu_Stats, StatsTabButton, StatsPageWidget});
    TabEntries.Add(FTabEntry{DemoGameplayTags::UI_PlayerMenu_Inventory, InventoryTabButton, InventoryPageWidget});

    InitMenu();

    CloseButton->OnClicked.AddDynamic(this, &UPlayerMenuWidget::HideMenu);
}

FReply UPlayerMenuWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Key(Tab) -> close
    if (InKeyEvent.GetKey() == EKeys::Tab)
    {
        ShowMenu(false);
        return FReply::Handled();
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UPlayerMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Key(Escape) -> close
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        ShowMenu(false);
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPlayerMenuWidget::ShowMenu(bool bShow)
{
    if (ADemoPlayerController* DemoPlayerController = Cast<ADemoPlayerController>(GetOwningPlayer()))
    {
        DemoPlayerController->ShowPlayerMenu(bShow);
    }
}