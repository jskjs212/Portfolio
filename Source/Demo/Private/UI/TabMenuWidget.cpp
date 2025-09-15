// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/TabMenuWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
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
        SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        SetFocusToWidget(TabEntries[PageSwitcher->GetActiveWidgetIndex()].Widget);
    }
    else
    {
        SetVisibility(ESlateVisibility::Collapsed);
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
        TabEntry.TabButton->OnTabButtonClicked.BindUObject(this, &ThisClass::HandleTabButtonClicked);
        TabEntry.TabButton->OnTabButtonHovered.BindUObject(this, &ThisClass::HandleTabButtonHovered);
        TabEntry.TabButton->OnTabButtonUnhovered.BindUObject(this, &ThisClass::HandleTabButtonUnhovered);
    }

    ActiveTabTag = TabEntries[0].Tag;

    // SelectTab() should avoid selecting already selected tab.
    // Initial index is already 0, so update tab button colors manually.
    for (int32 Index = 0; FTabEntry& TabEntry : TabEntries)
    {
        UpdateTabButtonColor(TabEntry, Index == 0, false);
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

    if (TabButtonClickSound)
    {
        UGameplayStatics::PlaySound2D(this, TabButtonClickSound);
    }

    // Already selected
    if (PageSwitcher->GetActiveWidgetIndex() == InIndex)
    {
        return;
    }

    PageSwitcher->SetActiveWidgetIndex(InIndex);
    ActiveTabTag = TabEntries[InIndex].Tag;
    CancelDragDrop();

    for (int32 Index = 0; FTabEntry& TabEntry : TabEntries)
    {
        // Update tab button colors
        const bool bIsTargetTab = Index == InIndex;
        if (bIsTargetTab)
        {
            SetFocusToWidget(TabEntry.Widget);
            //SetDesiredFocusWidget(TabEntry.Widget);
        }
        UpdateTabButtonColor(TabEntry, bIsTargetTab, false);
        Index++;
    }
}

void UTabMenuWidget::SelectTab(const FGameplayTag InTag)
{
    if (TabButtonClickSound)
    {
        UGameplayStatics::PlaySound2D(this, TabButtonClickSound);
    }

    if (ActiveTabTag == InTag)
    {
        return;
    }

    CancelDragDrop();

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
        UpdateTabButtonColor(TabEntry, bIsTargetTab, false);
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

void UTabMenuWidget::UpdateTabButtonColor(FTabEntry& InTabEntry, bool bActive, bool bHovered)
{
    const FLinearColor& NewColor = bHovered ? TabButtonHoveredColor : (bActive ? TabButtonActiveColor : TabButtonInactiveColor);

    if (bUseTabButtonImages && InTabEntry.Image)
    {
        InTabEntry.Image->SetColorAndOpacity(NewColor);
    }
    else
    {
        InTabEntry.TabButton->SetColorAndOpacity(NewColor);
    }
}

void UTabMenuWidget::CancelDragDrop()
{
    if (UWidgetBlueprintLibrary::IsDragDropping())
    {
        UWidgetBlueprintLibrary::CancelDragDrop();
    }
}

void UTabMenuWidget::HandleTabButtonHovered(const FGameplayTag InTag)
{
    if (TabButtonHoveredSound)
    {
        UGameplayStatics::PlaySound2D(this, TabButtonHoveredSound, 0.6f);
    }

    if (ActiveTabTag == InTag)
    {
        return;
    }

    for (FTabEntry& TabEntry : TabEntries)
    {
        if (TabEntry.Tag == InTag)
        {
            UpdateTabButtonColor(TabEntry, false, true);
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
            UpdateTabButtonColor(TabEntry, TabEntry.Tag == ActiveTabTag, false);
            break;
        }
    }
}