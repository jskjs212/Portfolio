// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoHUDWidget.generated.h"

class IInteractable;
class UCanvasPanelSlot;
class UImage;
class UInteractPromptWidget;
class UItemInfoWidget;
class UStatBarWidget;

/**
 * Demo HUD Widget
 * ResourceBar (Health, Stamina), Interact (ItemInfo, Prompt), Crosshair
 */
UCLASS()
class DEMO_API UDemoHUDWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeConstruct() override;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    // Update interact widgets.
    void UpdateInteractWidgets(IInteractable* Interactable);

    // Update target for lock-on.
    void UpdateTargetStatus(AActor* NewTarget);

    void UpdateLockOnPosition();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    /* Interaction */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemInfoWidget> ItemInfoWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UInteractPromptWidget> InteractPromptWidget;

    /* Stat bar */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UStatBarWidget> HealthBarWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UStatBarWidget> StaminaBarWidget;

    /* Targeting */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> LockOnMarker;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    TObjectPtr<AActor> LockedTarget;

    TObjectPtr<UCanvasPanelSlot> LockOnMarkerSlot;
};