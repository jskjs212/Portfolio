// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/TabMenuWidget.h"
#include "Components/WidgetSwitcher.h"
#include "UI/TabButton.h"

void UTabMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIsFocusable(true);

    checkf(PageSwitcher, TEXT("Failed to bind widgets."));
}

void UTabMenuWidget::InitMenu()
{
    if (TabEntries.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("UTabMenuWidget - No tab entries."));
        return;
    }

    // Bind tab button events
    for (FTabEntry& TabEntry : TabEntries)
    {
        TabEntry.TabButton->OnTabButtonClicked.BindUObject(this, &UTabMenuWidget::HandleTabButtonClicked);
        TabEntry.TabButton->OnTabButtonHovered.BindUObject(this, &UTabMenuWidget::HandleTabButtonHovered);
        TabEntry.TabButton->OnTabButtonUnhovered.BindUObject(this, &UTabMenuWidget::HandleTabButtonUnhovered);
    }

    // TEST:
    UE_LOG(LogTemp, Warning, TEXT("TEST: ActiveIndex: %d"), PageSwitcher->GetActiveWidgetIndex());

    ActiveTabTag = TabEntries[0].Tag;

    // SelectTab() should avoid selecting already selected tab.
    // Initial index is already 0, so update tab button colors manually.
    for (int32 Index = 0; FTabEntry& TabEntry : TabEntries)
    {
        const FLinearColor& TabButtonColor = Index == 0 ? TabButtonActiveColor : TabButtonInactiveColor;
        TabEntry.TabButton->SetColorAndOpacity(TabButtonColor);
        Index++;
    }
}

void UTabMenuWidget::SelectTab(const int32 InIndex)
{
    // Out of range
    if (InIndex < 0 || InIndex >= TabEntries.Num())
    {
        return;
    }

    // Already selected
    if (PageSwitcher->GetActiveWidgetIndex() == InIndex)
    {
        return;
    }

    PageSwitcher->SetActiveWidgetIndex(InIndex);
    ActiveTabTag = TabEntries[InIndex].Tag;

    for (int32 Index = 0; FTabEntry& TabEntry : TabEntries)
    {
        // Update tab button colors
        const FLinearColor& TabButtonColor = Index == InIndex ? TabButtonActiveColor : TabButtonInactiveColor;
        TabEntry.TabButton->SetColorAndOpacity(TabButtonColor);
        Index++;
    }
}

void UTabMenuWidget::SelectTab(const FGameplayTag InTag)
{
    if (ActiveTabTag == InTag)
    {
        return;
    }

    for (int32 Index = 0; FTabEntry& TabEntry : TabEntries)
    {
        const bool bIsTargetPage = TabEntry.Tag == InTag;
        if (bIsTargetPage)
        {
            PageSwitcher->SetActiveWidgetIndex(Index);
            ActiveTabTag = InTag;
        }
        // Update tab button colors
        const FLinearColor& TabButtonColor = bIsTargetPage ? TabButtonActiveColor : TabButtonInactiveColor;
        TabEntry.TabButton->SetColorAndOpacity(TabButtonColor);
        Index++;
    }
}

void UTabMenuWidget::HandleTabButtonHovered(const FGameplayTag InTag)
{
    if (ActiveTabTag == InTag)
    {
        return;
    }

    for (FTabEntry& TabEntry : TabEntries)
    {
        if (TabEntry.Tag == InTag)
        {
            TabEntry.TabButton->SetColorAndOpacity(TabButtonHoveredColor);
            break;
        }
    }
}

void UTabMenuWidget::HandleTabButtonUnhovered(const FGameplayTag InTag)
{
    if (ActiveTabTag == InTag)
    {
        return;
    }

    for (FTabEntry& TabEntry : TabEntries)
    {
        if (TabEntry.Tag == InTag)
        {
            const FLinearColor& TabButtonColor = TabEntry.Tag == ActiveTabTag ? TabButtonActiveColor : TabButtonInactiveColor;
            TabEntry.TabButton->SetColorAndOpacity(TabButtonColor);
            break;
        }
    }
}