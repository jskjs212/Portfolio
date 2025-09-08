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

    APawn* OwnerPawn = GetOwningPlayerPawn();
    if (OwnerPawn)
    {
        UStatsComponent* StatsComp = OwnerPawn->FindComponentByClass<UStatsComponent>();
        if (StatsComp)
        {
            const float CurrentHealth = StatsComp->GetCurrentHealth();
            CurrentHealthText->SetText(FText::AsNumber(CurrentHealth));
        }
    }
}