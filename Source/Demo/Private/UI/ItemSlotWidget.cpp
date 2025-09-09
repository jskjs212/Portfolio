// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ItemSlotWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "DemoTypes/TableRowBases.h"
#include "Items/ItemTypes.h"

void UItemSlotWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ItemBorder && ItemIcon
        && HoveredBorderTriangleImage && QuantitySizeBox,
        TEXT("Failed to bind widgets."));

    if (!NormalBorderImage || !HoveredBorderImage)
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemSlotWidget - Border image is not set."));
    }

    HandleUnhovered();
    UpdateVisuals();
}

void UItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    HandleHovered();
}

void UItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    HandleUnhovered();
}

void UItemSlotWidget::UpdateVisuals()
{
    // No item -> hide -> return
    if (!ItemSlot.IsValid())
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        QuantitySizeBox->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    // Yes item -> update (when changed)
    if (CurrentRowHandle != ItemSlot.RowHandle)
    {
        if (const FItemDataBase* ItemData = ItemSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UItemSlotWidget::UpdateVisuals")))
        {
            CurrentRowHandle = ItemSlot.RowHandle;
            ItemIcon->SetBrushFromTexture(ItemData->Icon);
        }
    }

    // Show (whether changed or not)
    ItemIcon->SetVisibility(ESlateVisibility::Visible);
    QuantitySizeBox->SetVisibility(ESlateVisibility::Visible);
    QuantityText->SetText(FText::AsNumber(ItemSlot.Quantity));
}

void UItemSlotWidget::HandleHovered()
{
    ItemBorder->SetBrushColor(HoveredBorderColor);
    ItemBorder->SetBrushFromTexture(HoveredBorderImage);
    HoveredBorderTriangleImage->SetVisibility(ESlateVisibility::Visible);
}

void UItemSlotWidget::HandleUnhovered()
{
    ItemBorder->SetBrushColor(NormalBorderColor);
    ItemBorder->SetBrushFromTexture(NormalBorderImage);
    HoveredBorderTriangleImage->SetVisibility(ESlateVisibility::Hidden);
}