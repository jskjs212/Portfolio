// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ContextMenuWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Framework/Application/SlateApplication.h"
#include "UI/TabButton.h"

void UContextMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(FirstDesignButton && MiddleDesignButton && LastDesignButton
        && ContextVerticalBox,
        TEXT("Failed to bind widgets."));

    FirstDesignButton->SetVisibility(ESlateVisibility::Collapsed);
    MiddleDesignButton->SetVisibility(ESlateVisibility::Collapsed);
    LastDesignButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UContextMenuWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    HideContextMenu();
}

void UContextMenuWidget::SetupActions(const TArray<FContextAction>& InActions)
{
    if (bSetupDone)
    {
        UE_LOG(LogTemp, Error, TEXT("UContextMenuWidget::SetupActions - Already setup."));
        return;
    }
    bSetupDone = true;

    ActionButtons.Empty();
    HoveredImages.Empty();

    for (int32 Index = 0; Index < InActions.Num(); ++Index)
    {
        UTabButton* NewButton = nullptr;

        // Create buttons based on action.
        if (Index == 0) // First
        {
            //NewButton = DuplicateObject<UTabButton>(FirstDesignButton, this);
            NewButton = FirstDesignButton;
        }
        else if (Index == InActions.Num() - 1) // Last
        {
            //NewButton = DuplicateObject<UTabButton>(LastDesignButton, this);
            NewButton = LastDesignButton;
        }
        else // Middle
        {
            NewButton = DuplicateObject<UTabButton>(MiddleDesignButton, this);
        }

        if (NewButton)
        {
            NewButton->SetTabTag(InActions[Index].ActionTag);
            NewButton->OnTabButtonHovered.BindUObject(this, &ThisClass::HandleButtonHovered);
            NewButton->OnTabButtonUnhovered.BindUObject(this, &ThisClass::HandleButtonUnhovered);
            NewButton->SetVisibility(ESlateVisibility::Visible);

            ContextVerticalBox->AddChild(NewButton);
            ActionButtons.Add(NewButton);

            UOverlay* Overlay = Cast<UOverlay>(NewButton->GetChildAt(0));
            if (!Overlay)
            {
                UE_LOG(LogTemp, Error, TEXT("UContextMenuWidget::SetupActions - Overlay is null for index %d"), Index);
                continue;
            }

            TArray<UWidget*> AllChildren = Overlay->GetAllChildren();
            for (UWidget* Child : AllChildren)
            {
                // Set button label
                if (UTextBlock* TextBlock = Cast<UTextBlock>(Child))
                {
                    TextBlock->SetText(InActions[Index].Label);
                }
                // Show/hide hover image
                else if (UImage* HoveredImage = Cast<UImage>(Child))
                {
                    HoveredImage->SetVisibility(ESlateVisibility::Hidden);
                    HoveredImages.Add(HoveredImage);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UContextMenuWidget::SetupActions - Failed to duplicate button for index %d"), Index);
        }
    }

    if (ActionButtons.Num() != 0 && ActionButtons.Num() != InActions.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("UContextMenuWidget::SetupActions - ActionButtons.Num() != InActions.Num()"));
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

    SetVisibility(ESlateVisibility::Hidden);
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
        if (ActionButtons[Index]->GetTabTag() == InTag)
        {
            HoveredImages[Index]->SetVisibility(ESlateVisibility::Visible);
            return;
        }
    }
}

void UContextMenuWidget::HandleButtonUnhovered(FGameplayTag InTag)
{
    for (int32 Index = 0; Index < ActionButtons.Num(); ++Index)
    {
        if (ActionButtons[Index]->GetTabTag() == InTag)
        {
            HoveredImages[Index]->SetVisibility(ESlateVisibility::Hidden);
            return;
        }
    }
}