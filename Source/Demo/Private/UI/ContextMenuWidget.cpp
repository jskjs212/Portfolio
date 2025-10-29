// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ContextMenuWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "DemoTypes/LogCategories.h"
#include "Framework/Application/SlateApplication.h"
#include "UI/Components/TagButton.h"

void UContextMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(FirstDesignButton && MiddleDesignButton && LastDesignButton && ContextVerticalBox, TEXT("Failed to bind widgets."));

    FirstDesignButton->SetVisibility(ESlateVisibility::Collapsed);
    MiddleDesignButton->SetVisibility(ESlateVisibility::Collapsed);
    LastDesignButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UContextMenuWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    HideContextMenu();
}

void UContextMenuWidget::InitActions(const TArray<FContextAction>& InActions)
{
    if (bSetupDone)
    {
        DemoLOG_CF(LogUI, Error, TEXT("Already setup."));
        return;
    }
    bSetupDone = true;

    const int32 NumActions = InActions.Num();
    ActionButtons.Empty(NumActions);
    HoveredBorders.Empty(NumActions);

    for (int32 Index = 0; Index < NumActions; ++Index)
    {
        UTagButton* NewButton = nullptr;

        // Create buttons based on action
        if (Index == 0) // First
        {
            NewButton = FirstDesignButton;
        }
        else if (Index == NumActions - 1) // Last
        {
            NewButton = LastDesignButton;
        }
        else // Middle
        {
            NewButton = DuplicateObject<UTagButton>(MiddleDesignButton, this);
        }

        if (NewButton)
        {
            UBorder* HoveredBorder = Cast<UBorder>(NewButton->GetChildAt(0));
            if (!HoveredBorder)
            {
                DemoLOG_CF(LogUI, Error, TEXT("HoveredBorder is null for index %d"), Index);
                continue;
            }

            UTextBlock* TextBlock = Cast<UTextBlock>(HoveredBorder->GetChildAt(0));
            if (!TextBlock)
            {
                DemoLOG_CF(LogUI, Error, TEXT("TextBlock is null for index %d"), Index);
                continue;
            }

            NewButton->SetTypeTag(InActions[Index].ActionTag);
            NewButton->OnTagButtonHovered.BindUObject(this, &ThisClass::HandleButtonHovered);
            NewButton->OnTagButtonUnhovered.BindUObject(this, &ThisClass::HandleButtonUnhovered);
            NewButton->SetVisibility(ESlateVisibility::Visible);

            ContextVerticalBox->AddChild(NewButton);
            ActionButtons.Add(NewButton);

            HoveredBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
            HoveredBorder->SetBrushColor(FLinearColor{1.f, 1.f, 1.f, 0.f}); // Hide border
            HoveredBorders.Add(HoveredBorder);

            TextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
            TextBlock->SetText(InActions[Index].Label);
        }
        else
        {
            DemoLOG_CF(LogUI, Error, TEXT("Failed to duplicate button for index %d"), Index);
        }
    }

    if (ActionButtons.Num() != NumActions)
    {
        DemoLOG_CF(LogUI, Error, TEXT("ActionButtons.Num() != InActions.Num()"));
    }
}

void UContextMenuWidget::ShowContextMenu()
{
    PreviousFocus = FSlateApplication::Get().GetUserFocusedWidget(0);

    SetPositionToCursor();
    SetVisibility(ESlateVisibility::Visible);
}

void UContextMenuWidget::HideContextMenu()
{
    if (!IsVisible())
    {
        return;
    }

    // Return focus
    if (PreviousFocus.IsValid())
    {
        FSlateApplication::Get().SetUserFocus(0, PreviousFocus.Pin(), EFocusCause::SetDirectly);
    }
    PreviousFocus = nullptr;

    SetVisibility(ESlateVisibility::Collapsed);
}

void UContextMenuWidget::SetPositionToCursor()
{
    const FVector2D& MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
    const FVector2D Offset = FVector2D(-5.f);
    const FVector2D AdjustedPosition = MousePosition + Offset;

    SetPositionInViewport(AdjustedPosition, false);
}

void UContextMenuWidget::HandleButtonHovered(FGameplayTag InTag)
{
    for (int32 Index = 0; Index < ActionButtons.Num(); ++Index)
    {
        if (ActionButtons[Index]->GetTypeTag() == InTag)
        {
            HoveredBorders[Index]->SetBrushColor(FLinearColor::White);
            return;
        }
    }
}

void UContextMenuWidget::HandleButtonUnhovered(FGameplayTag InTag)
{
    for (int32 Index = 0; Index < ActionButtons.Num(); ++Index)
    {
        if (ActionButtons[Index]->GetTypeTag() == InTag)
        {
            HoveredBorders[Index]->SetBrushColor(FLinearColor{1.f, 1.f, 1.f, 0.f});
            return;
        }
    }
}