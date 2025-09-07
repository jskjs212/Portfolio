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

    CurrentHealthText->SetText(FText::FromString(TEXT("0")));

    UE_LOG(LogTemp, Warning, TEXT("UStatsPageWidget - 0"));
    APawn* OwnerPawn = GetOwningPlayerPawn();
    if (OwnerPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("UStatsPageWidget - 1"));
        UStatsComponent* StatsComp = OwnerPawn->FindComponentByClass<UStatsComponent>();
        if (StatsComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("UStatsPageWidget - 2"));
            const float CurrentHealth = StatsComp->GetCurrentHealth();
            CurrentHealthText->SetText(FText::AsNumber(CurrentHealth));
        }
    }
}