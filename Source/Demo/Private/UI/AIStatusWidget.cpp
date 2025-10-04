// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AIStatusWidget.h"
#include "Components/StatsComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "DemoTypes/LogCategories.h"
#include "GameplayTagContainer.h"
#include "UI/StatBarWidget.h"

void UAIStatusWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(HealthBar && NameText, TEXT("Failed to bind widgets."));
}

void UAIStatusWidget::InitAIStatus(AActor* InOwnerActor)
{
    if (!InOwnerActor)
    {
        DemoLOG_CF(LogUI, Warning, TEXT("InOwnerActor is null."));
        return;
    }

    OwnerActor = InOwnerActor;
    HealthBar->InitStatBar(OwnerActor, UStatsComponent::HealthTag);
    NameText->SetText(FText::FromString(OwnerActor->GetName()));
}