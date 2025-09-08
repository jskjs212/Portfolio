// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InteractPromptWidget.h"
#include "Components/TextBlock.h"

void UInteractPromptWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ActionKeyWidget && ActionText, TEXT("Failed to bind widgets."));
}

void UInteractPromptWidget::SetActionText(const FText& InText)
{
    ActionText->SetText(InText);
}