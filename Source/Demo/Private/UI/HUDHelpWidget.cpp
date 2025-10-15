// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUDHelpWidget.h"
#include "Components/TextBlock.h"

void UHUDHelpWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(HelpText, TEXT("Failed to bind HelpTextBlock."));

    HelpText->SetVisibility(ESlateVisibility::Collapsed);
}

void UHUDHelpWidget::ToggleHelpText()
{
    bIsHelpTextVisible ^= true;
    ESlateVisibility NewVisibility = bIsHelpTextVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
    HelpText->SetVisibility(NewVisibility);
}