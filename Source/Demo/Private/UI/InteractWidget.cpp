// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InteractWidget.h"
#include "UI/ItemInfoWidget.h"

void UInteractWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!ItemInfo)
    {
        UE_LOG(LogTemp, Error, TEXT("UInteractWidget - Failed to bind widgets."));
        return;
    }
}

void UInteractWidget::UpdateUI(const FItemSlot& InSlot)
{
    if (!InSlot.IsValid())
    {
        SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    ItemInfo->UpdateUI(InSlot);
}