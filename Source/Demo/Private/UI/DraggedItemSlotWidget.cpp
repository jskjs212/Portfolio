// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DraggedItemSlotWidget.h"
#include "Components/Image.h"

void UDraggedItemSlotWidget::SetItemIcon(UTexture2D* InIcon, float InOpacity)
{
    if (ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(InIcon);
        //ItemIcon->SetColorAndOpacity(FLinearColor{1.f, 1.f, 1.f, InOpacity});
        ItemIcon->SetOpacity(InOpacity);
    }
}