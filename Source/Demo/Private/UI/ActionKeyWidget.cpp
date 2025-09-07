// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ActionKeyWidget.h"
#include "Components/TextBlock.h"

void UActionKeyWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(KeyText, TEXT("Failed to bind widgets."));
}

void UActionKeyWidget::SetKeyText(const FText& InText)
{
    if (KeyText)
    {
        KeyText->SetText(InText);
    }
}