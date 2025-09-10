// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ContextMenuWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UContextMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(FirstDesignButton && MiddleDesignButton && LastDesignButton
        && ContextVerticalBox,
        TEXT("Failed to bind widgets."));

    //FirstButtonStyle = FirstDesignButton->GetStyle();
    //MiddleButtonStyle = MiddleDesignButton->GetStyle();
    //LastButtonStyle = LastDesignButton->GetStyle();

    FirstDesignButton->SetVisibility(ESlateVisibility::Collapsed);
    MiddleDesignButton->SetVisibility(ESlateVisibility::Collapsed);
    LastDesignButton->SetVisibility(ESlateVisibility::Collapsed);

    //FirstDesignButton->RemoveFromParent();
    //MiddleDesignButton->RemoveFromParent();
    //LastDesignButton->RemoveFromParent();
}

void UContextMenuWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    HideContextMenu();
}

void UContextMenuWidget::SetupActions(const TArray<FContextAction>& InActions)
{
    ActionButtons.Empty();
    HoveredImages.Empty();

    for (int32 Index = 0; Index < InActions.Num(); ++Index)
    {
        UButton* NewButton = nullptr;

        // Create buttons based on action.
        if (Index == 0) // First
        {
            NewButton = DuplicateObject<UButton>(FirstDesignButton, this);
        }
        else if (Index == InActions.Num() - 1) // Last
        {
            NewButton = DuplicateObject<UButton>(LastDesignButton, this);
        }
        else // Middle
        {
            NewButton = DuplicateObject<UButton>(MiddleDesignButton, this);
        }

        if (NewButton)
        {
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
                    NewButton->OnHovered.AddDynamic(this, &UContextMenuWidget::HandleButtonHovered);
                    NewButton->OnUnhovered.AddDynamic(this, &UContextMenuWidget::HandleButtonUnhovered);
                    HoveredImages.Add(HoveredImage);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UContextMenuWidget::SetupActions - Failed to duplicate button for index %d"), Index);
        }
    }

    if (ActionButtons.Num() != InActions.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("UContextMenuWidget::SetupActions - ActionButtons.Num() != InActions.Num()"));
    }
}

void UContextMenuWidget::ShowContextMenu(const FItemSlot& InSlot, int32 DesignatedIndex)
{
    SetActionRequest(InSlot, DesignatedIndex);
    SetPositionToCursor();
    SetVisibility(ESlateVisibility::Visible);
}

void UContextMenuWidget::SetPositionToCursor()
{
    const float DPI = UWidgetLayoutLibrary::GetViewportScale(this);
    const FVector2D& MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
    const FVector2D Offset = FVector2D(10.f);
    const FVector2D AdjustedPosition = MousePosition * DPI - Offset;

    SetPositionInViewport(AdjustedPosition, true);
}

void UContextMenuWidget::HandleButtonHovered()
{
    // TODO: Optimize
    for (int32 Index = 0; Index < ActionButtons.Num(); ++Index)
    {
        if (ActionButtons[Index]->IsHovered())
        {
            HoveredImages[Index]->SetVisibility(ESlateVisibility::Visible);
            return;
        }
    }
}

void UContextMenuWidget::HandleButtonUnhovered()
{
    // TODO: Optimize
    for (int32 Index = 0; Index < ActionButtons.Num(); ++Index)
    {
        if (!ActionButtons[Index]->IsHovered())
        {
            HoveredImages[Index]->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}