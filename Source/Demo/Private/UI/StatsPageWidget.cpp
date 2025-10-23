// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/StatsPageWidget.h"
#include "Components/StatsComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"

void UStatsPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(CurrentHealthText && CurrentStaminaText
        && STRText && DEXText && INTText
        && AttackText && DefenseText,
        TEXT("Failed to bind widgets."));

    // @hardcoded - DataTable or UMyTagText that contains tag.
    StatTagToTextMap = TMap<FGameplayTag, TObjectPtr<UTextBlock>>{
        {UStatsComponent::HealthTag, CurrentHealthText},
        {UStatsComponent::StaminaTag, CurrentStaminaText},
        {UStatsComponent::STRTag, STRText},
        {UStatsComponent::DEXTag, DEXText},
        {UStatsComponent::INTTag, INTText},
        {UStatsComponent::AttackTag, AttackText},
        {UStatsComponent::DefenseTag, DefenseText}
    };

    BindToStatsComponent();
}

//void UStatsPageWidget::ProcessPendingUpdates()
//{
//    if (APawn* OwningPawn = GetOwningPlayerPawn())
//    {
//        if (UStatsComponent* StatsComponent = OwningPawn->FindComponentByClass<UStatsComponent>())
//        {
//        }
//    }
//}

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
    //if (!IsVisible())
    //{
    //    PendingUpdateStatTags.AddUnique(StatTag);
    //    return;
    //}

    TObjectPtr<UTextBlock>* TextBlockPtr = StatTagToTextMap.Find(StatTag);
    if (TextBlockPtr)
    {
        (*TextBlockPtr)->SetText(FText::AsNumber(FMath::RoundToInt32(NewValue)));
    }
}