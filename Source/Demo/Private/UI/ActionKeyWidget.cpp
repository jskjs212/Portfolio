// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ActionKeyWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UActionKeyWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(KeyTextBlock && BackgroundImage, TEXT("Failed to bind widgets."));
}

void UActionKeyWidget::NativePreConstruct()
{
    if (!KeyText.IsEmpty())
    {
        KeyTextBlock->SetText(KeyText);
    }

    if (BackgroundTexture)
    {
        BackgroundImage->SetBrushFromTexture(BackgroundTexture);
    }
}

void UActionKeyWidget::SetKeyText(const FText& InText)
{
    if (KeyTextBlock)
    {
        KeyTextBlock->SetText(InText);
    }
}