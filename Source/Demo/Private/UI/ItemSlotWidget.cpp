// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ItemSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "DemoTypes/TableRowBases.h"
#include "Items/ItemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/SlateBrush.h"
#include "UI/DraggedItemSlotWidget.h"
#include "UI/ItemSlotDragDropOp.h"

void UItemSlotWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ItemBorder && ItemIcon && HoveredBorderTriangleImage
        && QuantitySizeBox && QuantityText,
        TEXT("Failed to bind widgets."));

    if (!NormalBorderImage || !HoveredBorderImage || !DraggedItemSlotWidgetBPClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("UItemSlotWidget - Properties are not set."));
    }

    HandleUnhovered();
    UpdateVisuals();
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (ItemSlot.IsValid())
    {
        // LMB -> Drag drop
        if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        {
            if (CanDragDrop())
            {
                return FReply::Unhandled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
            }
        }

        // RMB -> Show context menu
        if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
        {
            OnRightClicked.ExecuteIfBound(ItemSlot, Index);
            return FReply::Handled();
        }
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
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

FReply UItemSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (ItemSlot.IsValid())
    {
        // LMB -> Use item
        if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        {
            OnLeftDoubleClicked.ExecuteIfBound(ItemSlot, Index);
            return FReply::Handled();
        }
    }

    return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void UItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    // Create drag visual (widget)
    UDraggedItemSlotWidget* DraggedItemSlotWidget = CreateWidget<UDraggedItemSlotWidget>(this, DraggedItemSlotWidgetBPClass);
    if (DraggedItemSlotWidget)
    {
        // Create drag drop operation
        UItemSlotDragDropOp* DragDropOp = Cast<UItemSlotDragDropOp>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemSlotDragDropOp::StaticClass()));
        if (DragDropOp)
        {
            // Setup OutOperation
            const float Opacity = 0.7f;
            const FVector2D DesiredSize = ItemIcon->GetDesiredSize();
            UTexture2D* ItemIconResource = Cast<UTexture2D>(ItemIcon->GetBrush().GetResourceObject());
            if (!ItemIconResource)
            {
                UE_LOG(LogTemp, Error, TEXT("UItemSlotWidget::NativeOnDragDetected - ItemIconResource is null."));
            }

            DraggedItemSlotWidget->SetItemIcon(ItemIconResource, Opacity);
            DraggedItemSlotWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            DraggedItemSlotWidget->SetDesiredSizeInViewport(DesiredSize);
            DragDropOp->DefaultDragVisual = DraggedItemSlotWidget;
            DragDropOp->Setup(ItemSlot, Index);
            OutOperation = DragDropOp;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UItemSlotWidget::NativeOnDragDetected - Failed to create UItemSlotDragDropOp."));
            DraggedItemSlotWidget->RemoveFromParent();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UItemSlotWidget::NativeOnDragDetected - Failed to create UDraggedItemSlotWidget."));
    }
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UItemSlotDragDropOp* DragDropOp = Cast<UItemSlotDragDropOp>(InOperation);
    if (DragDropOp)
    {
        const int32 SrcIndex = DragDropOp->GetIndex();
        const FItemSlot& SrcSlot = DragDropOp->GetItemSlot();

        OnDropped.ExecuteIfBound(SrcSlot, SrcIndex, ItemSlot, Index);

        return true;
    }

    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UItemSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // Don't have to remove (GC)
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

void UItemSlotWidget::UpdateVisuals()
{
    // No item -> hide -> return
    if (!ItemSlot.IsValid())
    {
        ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
        QuantitySizeBox->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    // Yes item -> update (when changed)
    if (CurrentItemRowHandle != ItemSlot.RowHandle)
    {
        if (const FItemDataBase* ItemData = ItemSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UItemSlotWidget::UpdateVisuals")))
        {
            CurrentItemRowHandle = ItemSlot.RowHandle;
            ItemIcon->SetBrushFromTexture(ItemData->Icon);
        }
    }

    // Show (whether changed or not)
    ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
    QuantitySizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
    QuantityText->SetText(FText::AsNumber(ItemSlot.Quantity));
}

void UItemSlotWidget::HandleHovered()
{
    if (HoveredSound)
    {
        UGameplayStatics::PlaySound2D(this, HoveredSound, 0.5f);
    }

    if (ItemSlot.IsValid())
    {
        OnHovered.ExecuteIfBound(ItemSlot);
    }
    ItemBorder->SetBrushColor(HoveredBorderColor);
    ItemBorder->SetBrushFromTexture(HoveredBorderImage);
    HoveredBorderTriangleImage->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UItemSlotWidget::HandleUnhovered()
{
    OnUnhovered.ExecuteIfBound();
    ItemBorder->SetBrushColor(NormalBorderColor);
    ItemBorder->SetBrushFromTexture(NormalBorderImage);
    HoveredBorderTriangleImage->SetVisibility(ESlateVisibility::Collapsed);
}