// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/TabMenuWidget.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "UI/TabButton.h"

void UTabMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIsFocusable(true);

    checkf(PageSwitcher, TEXT("Failed to bind widgets."));
}

void UTabMenuWidget::SetVisibilityAndFocus(bool bShow)
{
    if (bShow)
    {
        SetVisibility(ESlateVisibility::Visible);
        SetFocusToWidget(TabEntries[PageSwitcher->GetActiveWidgetIndex()].Widget);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
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

    ActiveTabTag = TabEntries[0].Tag;

    // SelectTab() should avoid selecting already selected tab.
    // Initial index is already 0, so update tab button colors manually.
    for (int32 Index = 0; FTabEntry& TabEntry : TabEntries)
    {
        const FLinearColor& TabButtonColor = Index == 0 ? TabButtonActiveColor : TabButtonInactiveColor;
        UpdateTabButtonColor(TabEntry, TabButtonColor);
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
        const bool bIsTargetTab = Index == InIndex;
        if (bIsTargetTab)
        {
            SetFocusToWidget(TabEntry.Widget);
            //SetDesiredFocusWidget(TabEntry.Widget);
        }
        const FLinearColor& TabButtonColor = bIsTargetTab ? TabButtonActiveColor : TabButtonInactiveColor;
        UpdateTabButtonColor(TabEntry, TabButtonColor);
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
        const bool bIsTargetTab = TabEntry.Tag == InTag;
        if (bIsTargetTab)
        {
            PageSwitcher->SetActiveWidgetIndex(Index);
            ActiveTabTag = InTag;
            SetFocusToWidget(TabEntry.Widget);
            //SetDesiredFocusWidget(TabEntry.Widget);
        }
        // Update tab button colors
        const FLinearColor& TabButtonColor = bIsTargetTab ? TabButtonActiveColor : TabButtonInactiveColor;
        UpdateTabButtonColor(TabEntry, TabButtonColor);
        Index++;
    }
}

void UTabMenuWidget::SetFocusToWidget(UWidget* InWidget)
{
    if (UUserWidget* TargetWidget = Cast<UUserWidget>(InWidget))
    {
        if (TargetWidget->IsFocusable())
        {
            TargetWidget->SetFocus();
        }
    }
}

void UTabMenuWidget::UpdateTabButtonColor(FTabEntry& InTabEntry, const FLinearColor& InColor)
{
    if (bUseTabButtonImages && InTabEntry.Image)
    {
        InTabEntry.Image->SetColorAndOpacity(InColor);
    }
    else
    {
        InTabEntry.TabButton->SetColorAndOpacity(InColor);
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
            UpdateTabButtonColor(TabEntry, TabButtonHoveredColor);
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
            UpdateTabButtonColor(TabEntry, TabButtonColor);
            break;
        }
    }
}