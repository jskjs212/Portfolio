// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AIStatusWidget.h"
#include "Character/BaseCharacter.h"
#include "Components/TextBlock.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "UI/StatBarWidget.h"

void UAIStatusWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(HealthBar && NameText, TEXT("Failed to bind widgets."));
}

void UAIStatusWidget::BindToActor(AActor* InActor)
{
    if (!InActor)
    {
        DemoLOG_CF(LogUI, Warning, TEXT("InActor is null."));
        return;
    }

    HealthBar->BindToStatsComponent(InActor, DemoGameplayTags::Stat_Resource_Health);

    const ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(InActor);
    FText DisplayName = BaseCharacter ? BaseCharacter->GetCharacterDisplayName() : FText::FromString(InActor->GetName());
    NameText->SetText(DisplayName);
}