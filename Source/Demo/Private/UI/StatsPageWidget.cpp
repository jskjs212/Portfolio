// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StatsPageWidget.h"
#include "Components/StatsComponent.h"
#include "Components/TextBlock.h"

void UStatsPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!CurrentHealthText)
    {
        return;
    }
}