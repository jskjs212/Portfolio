// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StatModifierWidget.h"
#include "Components/StatsComponent.h"
#include "Components/TextBlock.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/StatsTypes.h"

void UStatModifierWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(StatNameText && StatValueText, TEXT("Failed to bind widgets."));
}

void UStatModifierWidget::Update(FGameplayTag StatTag, const FStatModifier& InStatModifier)
{
    // @hardcoded - DataTable
    static const TMap<FGameplayTag, FText> StatTagToNameMap = TMap<FGameplayTag, FText>{
        {UStatsComponent::STRTag, FText::FromString("STR")},
        {UStatsComponent::DEXTag, FText::FromString("DEX")},
        {UStatsComponent::INTTag, FText::FromString("INT")},
        {UStatsComponent::AttackTag, FText::FromString("Atk")},
        {UStatsComponent::DefenseTag, FText::FromString("Def")}
    };

    const FText* StatName = StatTagToNameMap.Find(StatTag);
    if (StatName)
    {
        const float PercentMultiplier = (InStatModifier.Operation == EStatModOp::Multiply) ? 100.f : 1.f;
        const FString Sign = (InStatModifier.Magnitude >= 0.f) ? TEXT("+") : TEXT("");
        //const FString Magnitude = FString::SanitizeFloat(InStatModifier.Magnitude * PercentMultiplier, 1);
        const FString Magnitude = FString::Printf(TEXT("%d"), FMath::RoundToInt32(InStatModifier.Magnitude * PercentMultiplier));
        const FString Percent = (InStatModifier.Operation == EStatModOp::Multiply) ? TEXT("%") : TEXT("");

        StatNameText->SetText(*StatName);
        StatValueText->SetText(FText::FromString(Sign + Magnitude + Percent));
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("Invalid StatTag %s."), *StatTag.ToString());
        StatNameText->SetText(FText::FromString("Error"));
    }
}