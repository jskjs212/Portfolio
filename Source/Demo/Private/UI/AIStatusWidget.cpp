// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AIStatusWidget.h"
#include "Character/BaseCharacter.h"
#include "Components/TextBlock.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "UI/StatBarWidget.h"

UAIStatusWidget::UAIStatusWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
    DamageTextHideTimerDelegate{FTimerDelegate::CreateUObject(this, &ThisClass::HideDamageText)}
{
}

void UAIStatusWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(HealthBar && NameText && DamageText, TEXT("Failed to bind widgets."));

    if (DamageTextDuration <= 0.f)
    {
        DemoLOG_CF(LogUI, Warning, TEXT("DamageTextDuration <= 0."));
    }

    NameText->SetVisibility(ESlateVisibility::HitTestInvisible);
    DamageText->SetVisibility(ESlateVisibility::Collapsed);
}

void UAIStatusWidget::BindToActor(AActor* InActor)
{
    if (!InActor)
    {
        DemoLOG_CF(LogUI, Warning, TEXT("InActor is null."));
        return;
    }

    if (CachedActor.IsValid())
    {
        // Already bound
        if (CachedActor.Get() == InActor)
        {
            return;
        }

        // Unbind from previous
        CachedActor->OnTakeAnyDamage.RemoveAll(this);
    }

    CachedActor = InActor;

    // Health bar
    HealthBar->BindToStatsComponent(InActor, DemoGameplayTags::Stat_Resource_Health);

    // Name text
    ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(InActor);
    FText DisplayName = BaseCharacter ? BaseCharacter->GetCharacterDisplayName() : FText::FromString(InActor->GetName());
    NameText->SetText(DisplayName);

    // Damage text
    InActor->OnTakeAnyDamage.AddDynamic(this, &ThisClass::HandleTakeAnyDamage);
}

void UAIStatusWidget::UnbindFromActor()
{
    if (CachedActor.IsValid())
    {
        CachedActor->OnTakeAnyDamage.RemoveAll(this);
        CachedActor = nullptr;
        HealthBar->UnbindFromStatsComponent();
    }
}

void UAIStatusWidget::HideDamageText()
{
    DamageText->SetVisibility(ESlateVisibility::Collapsed);
}

void UAIStatusWidget::HandleTakeAnyDamage(AActor* /* DamagedActor */, float Damage, const UDamageType* /* DamageType */, AController* /* InstigatorController */, AActor* /* DamageCauser */)
{
    // @TODO - Handle with DamageTypes. Add animation or effect.
    if (Damage > 0.f)
    {
        DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Damage)));
        DamageText->SetVisibility(ESlateVisibility::HitTestInvisible);

        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(DamageTextHideTimerHandle, DamageTextHideTimerDelegate, DamageTextDuration, false);
        }
    }
}