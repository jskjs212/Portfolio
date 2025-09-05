// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InteractPromptWidget.h"
#include "Components/TextBlock.h"

void UInteractPromptWidget::SetActionText(const FText& InText)
{
    if (ActionText)
    {
        ActionText->SetText(InText);
    }
}