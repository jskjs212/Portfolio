// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StatBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/LogCategories.h"
#include "Styling/SlateTypes.h"

void UStatBarWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(StatBar, TEXT("Failed to bind widgets."));

    StatBar->SetVisibility(ESlateVisibility::HitTestInvisible);
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

void UStatBarWidget::BindToStatsComponent(AActor* InActor, FGameplayTag InStatTag)
{
    if (!InActor)
    {
        DemoLOG_CF(LogUI, Error, TEXT("InActor is null."));
        return;
    }

    UStatsComponent* StatsComponent = InActor->FindComponentByClass<UStatsComponent>();
    if (!StatsComponent)
    {
        DemoLOG_CF(LogUI, Error, TEXT("Failed to find StatsComponent from %s."), *InActor->GetName());
        return;
    }

    if (!StatsComponent->HasStatType(InStatTag))
    {
        DemoLOG_CF(LogUI, Error, TEXT("StatsComponent doesn't have stat type: %s, owner: %s"), *InStatTag.ToString(), *InActor->GetName());
        return;
    }

    if (CachedStatsComponent.IsValid())
    {
        // Already bound
        if (CachedStatsComponent.Get() == StatsComponent && StatTag == InStatTag)
        {
            return;
        }

        // Unbind from previous
        CachedStatsComponent->OnCurrentResourceStatChanged.Remove(StatChangedDelegateHandle);
        StatChangedDelegateHandle.Reset();
    }

    StatTag = InStatTag;
    CachedStatsComponent = StatsComponent;

    // Bind
    StatChangedDelegateHandle = StatsComponent->OnCurrentResourceStatChanged.AddUObject(this, &ThisClass::HandleCurrentStatChanged);

    UpdateStatBar();
}

void UStatBarWidget::UnbindFromStatsComponent()
{
    if (CachedStatsComponent.IsValid())
    {
        CachedStatsComponent->OnCurrentResourceStatChanged.Remove(StatChangedDelegateHandle);
        CachedStatsComponent = nullptr;
        StatChangedDelegateHandle.Reset();
    }
}

void UStatBarWidget::UpdateStatBar()
{
    if (CachedStatsComponent.IsValid())
    {
        const float Current = CachedStatsComponent->GetCurrentResourceStatChecked(StatTag);
        const float Max = CachedStatsComponent->GetMaxResourceStatChecked(StatTag);
        const float Percent = (Max != 0.f) ? (Current / Max) : 0.f;

        StatBar->SetPercent(Percent);
    }
}