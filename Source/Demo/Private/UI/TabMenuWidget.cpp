// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/TabMenuWidget.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "DemoTypes/LogCategories.h"
#include "PlayerController/DemoPlayerController.h"
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

void UTabMenuWidget::SelectTab(const int32 InIndex)
{
    // Out of range
    if (InIndex < 0 || InIndex >= TabEntries.Num())
    {
        return;
    }

    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(this, DemoSoundTags::UI_TabButton_Click);
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
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(this, DemoSoundTags::UI_TabButton_Click);
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

void UTabMenuWidget::InitTabMenu()
{
    if (TabEntries.Num() == 0)
    {
        DemoLOG_CF(LogUI, Warning, TEXT("No tab entries."));
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
    const FLinearColor& NewColor = bActive ? TabButtonActiveColor : (bHovered ? TabButtonHoveredColor : TabButtonInactiveColor);

    if (bUseTabButtonImages)
    {
        if (InTabEntry.Image)
        {
            InTabEntry.Image->SetColorAndOpacity(NewColor);
        }
        else
        {
            DemoLOG_CF(LogUI, Warning, TEXT("TabEntry.Image is not set."));
        }
    }
    else
    {
        InTabEntry.TabButton->SetBackgroundColor(NewColor);
    }
}

void UTabMenuWidget::CancelDragDrop()
{
    if (UWidgetBlueprintLibrary::IsDragDropping())
    {
        UWidgetBlueprintLibrary::CancelDragDrop();

        if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
        {
            DemoPlayerController->SetCursorState(ECursorState::Default);
        }
    }
}

ADemoPlayerController* UTabMenuWidget::GetDemoPlayerController()
{
    if (!CachedDemoPlayerController.IsValid())
    {
        CachedDemoPlayerController = GetOwningPlayer<ADemoPlayerController>();
    }
    return CachedDemoPlayerController.Get();
}

void UTabMenuWidget::HandleTabButtonClicked(FGameplayTag InTag)
{
    SelectTab(InTag);

    if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
    {
        DemoPlayerController->SetCursorState(ECursorState::Default);
    }
}

void UTabMenuWidget::HandleTabButtonHovered(const FGameplayTag InTag)
{
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(this, DemoSoundTags::UI_TabButton_Hover);
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

    if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
    {
        DemoPlayerController->SetCursorState(ECursorState::Hovering);
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

    if (ADemoPlayerController* DemoPlayerController = GetDemoPlayerController())
    {
        DemoPlayerController->SetCursorState(ECursorState::Default);
    }
}