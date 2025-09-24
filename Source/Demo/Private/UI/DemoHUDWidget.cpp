// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DemoHUDWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/StatsComponent.h"
#include "Interfaces/Interactable.h"
#include "Interfaces/TargetInterface.h"
#include "Items/Item.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ActionKeyWidget.h"
#include "UI/InteractPromptWidget.h"
#include "UI/ItemInfoWidget.h"
#include "UI/StatBarWidget.h"

void UDemoHUDWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ItemInfoWidget && InteractPromptWidget
        && HealthBarWidget && StaminaBarWidget && LockOnMarker,
        TEXT("Failed to bind widgets."));

    // Hide interact widgets
    ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
    InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
    LockOnMarker->SetVisibility(ESlateVisibility::Collapsed);
}

void UDemoHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Init stat bars
    APawn* OwnerPawn = GetOwningPlayerPawn();
    HealthBarWidget->InitStatBar(OwnerPawn, UStatsComponent::HealthTag);
    StaminaBarWidget->InitStatBar(OwnerPawn, UStatsComponent::StaminaTag);

    // Cache slot
    LockOnMarkerSlot = Cast<UCanvasPanelSlot>(LockOnMarker->Slot);
    if (!LockOnMarkerSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("UDemoHUDWidget::NativeConstruct - LockOnMarkerSlot is not valid."));
    }
}

void UDemoHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (LockedTarget)
    {
        UpdateLockOnPosition();
    }
}

void UDemoHUDWidget::UpdateInteractWidgets(IInteractable* Interactable)
{
    AItem* Item = Cast<AItem>(Interactable);

    if (Item && Item->GetItemSlot().IsValid())
    {
        ItemInfoWidget->UpdateItemInfo(Item->GetItemSlot());
        ItemInfoWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

        // @hardcoded - Action text and key
        InteractPromptWidget->SetActionText(FText::FromString(TEXT("Pick Up")));
        InteractPromptWidget->ActionKeyWidget->SetKeyText(FText::FromString(TEXT("E")));
        InteractPromptWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    else
    {
        ItemInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
        InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDemoHUDWidget::UpdateTargetStatus(AActor* NewTarget)
{
    LockedTarget = NewTarget;

    if (LockedTarget)
    {
        LockOnMarker->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    else
    {
        LockOnMarker->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDemoHUDWidget::UpdateLockOnPosition()
{
    const FVector TargetLocation = Cast<ITargetInterface>(LockedTarget)->GetTargetPointLocation();
    FVector2D ScreenPosition;

    bool bProjected = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), TargetLocation, ScreenPosition);
    if (bProjected)
    {
        const float DPI = UWidgetLayoutLibrary::GetViewportScale(this);
        LockOnMarkerSlot->SetPosition(ScreenPosition / DPI);
    }
}