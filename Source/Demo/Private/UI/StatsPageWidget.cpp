// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StatsPageWidget.h"
#include "Components/StatsComponent.h"
#include "Components/TextBlock.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "GameFramework/Pawn.h"

void UStatsPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(CurrentHealthText && MaxHealthText && CurrentStaminaText && MaxStaminaText
        && STRText && DEXText && INTText && AttackText && DefenseText,
        TEXT("Failed to bind widgets."));

    // @hardcoded - DataTable or UMyTagText that contains tag.
    StatTagToTextMap = TMap<FGameplayTag, TObjectPtr<UTextBlock>>{
        {DemoGameplayTags::Stat_Resource_Health, CurrentHealthText},
        {DemoGameplayTags::Stat_Resource_Stamina, CurrentStaminaText},
        {DemoGameplayTags::Stat_Primary_STR, STRText},
        {DemoGameplayTags::Stat_Primary_DEX, DEXText},
        {DemoGameplayTags::Stat_Primary_INT, INTText},
        {DemoGameplayTags::Stat_Derived_Attack, AttackText},
        {DemoGameplayTags::Stat_Derived_Defense, DefenseText}
    };

    BindToStatsComponent();
}

void UStatsPageWidget::UpdateAllStats()
{
    if (APawn* OwningPawn = GetOwningPlayerPawn())
    {
        if (UStatsComponent* StatsComponent = OwningPawn->FindComponentByClass<UStatsComponent>())
        {
            for (auto& [StatTag, StatText] : StatTagToTextMap)
            {
                const auto [bFound, StatValue] = StatsComponent->GetStatFinalValue(StatTag);
                StatText->SetText(FText::AsNumber(FMath::RoundToInt32(bFound ? StatValue : 0.f)));
            }

            const float MaxHealth = StatsComponent->GetMaxHealth();
            const float MaxStamina = StatsComponent->GetMaxResourceStatChecked(DemoGameplayTags::Stat_Resource_Stamina);
            MaxHealthText->SetText(FText::AsNumber(FMath::RoundToInt32(MaxHealth)));
            MaxStaminaText->SetText(FText::AsNumber(FMath::RoundToInt32(MaxStamina)));
        }
    }
}

void UStatsPageWidget::BindToStatsComponent()
{
    if (APawn* OwningPawn = GetOwningPlayerPawn())
    {
        if (UStatsComponent* StatsComponent = OwningPawn->FindComponentByClass<UStatsComponent>())
        {
            StatsComponent->OnCurrentResourceStatChanged.AddUObject(this, &ThisClass::UpdateStatText);
            StatsComponent->OnPrimaryStatChanged.AddUObject(this, &ThisClass::UpdateStatText);
            StatsComponent->OnDerivedStatChanged.AddUObject(this, &ThisClass::UpdateStatText);
        }
    }
}

void UStatsPageWidget::UpdateStatText(FGameplayTag StatTag, float OldValue, float NewValue)
{
    TObjectPtr<UTextBlock>* TextBlockPtr = StatTagToTextMap.Find(StatTag);
    if (TextBlockPtr)
    {
        (*TextBlockPtr)->SetText(FText::AsNumber(FMath::RoundToInt32(NewValue)));
    }
}