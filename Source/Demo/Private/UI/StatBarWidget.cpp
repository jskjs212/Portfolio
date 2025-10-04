// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StatBarWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/LogCategories.h"
#include "GameFramework/Pawn.h"

void UStatBarWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(StatBar, TEXT("Failed to bind widgets."));
}

void UStatBarWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (StatBar)
    {
        // Apply appearance
        StatBar->SetFillColorAndOpacity(StatBarFillColor);

        if (StatBarFillImage || StatBarBackgroundImage)
        {
            FProgressBarStyle Style = StatBar->GetWidgetStyle();
            if (StatBarFillImage)
            {
                Style.FillImage.SetResourceObject(StatBarFillImage);
            }
            if (StatBarBackgroundImage)
            {
                Style.BackgroundImage.SetResourceObject(StatBarBackgroundImage);
            }
            StatBar->SetWidgetStyle(Style);
        }
    }
}

void UStatBarWidget::InitStatBar(AActor* OwnerActor, FGameplayTag InStatTag)
{
    if (OwnerActor)
    {
        StatTag = InStatTag;
        StatsComponent = OwnerActor->FindComponentByClass<UStatsComponent>();

        if (!StatsComponent)
        {
            DemoLOG_CF(LogUI, Error, TEXT("Failed to find StatsComponent from %s."), *OwnerActor->GetName());
            return;
        }
        if (!StatsComponent->HasStatType(StatTag))
        {
            DemoLOG_CF(LogUI, Error, TEXT("StatsComponent doesn't have stat type: %s, owner: %s"), *StatTag.ToString(), *OwnerActor->GetName());
            return;
        }

        // Bind
        StatsComponent->OnCurrentResourceStatChanged.AddUObject(this, &ThisClass::OnStatChanged);

        UpdateStatBar();
    }
}

void UStatBarWidget::UpdateStatBar()
{
    if (!StatsComponent)
    {
        return;
    }

    const float Current = StatsComponent->GetCurrentResourceStatChecked(StatTag);
    const float Max = StatsComponent->GetMaxResourceStatChecked(StatTag);
    const float Percent = (Max != 0.f) ? (Current / Max) : 0.f;

    StatBar->SetPercent(Percent);
}