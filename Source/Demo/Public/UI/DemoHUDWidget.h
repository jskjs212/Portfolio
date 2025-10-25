// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoHUDWidget.generated.h"

class IInteractable;
class UAIStatusWidget;
class UHUDHelpWidget;
class UInteractPromptWidget;
class UItemInfoWidget;
class UStatBarWidget;
class UTextBlock;

/**
 * Demo HUD Widget
 * ResourceBar (Health, Stamina, BossAIStatus), Interact (ItemInfo, Prompt), Crosshair
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

public:
    // Update interact widgets.
    void UpdateInteractWidgets(IInteractable* Interactable);

    // Hide boss AI status if BossActor is nullptr.
    void ShowBossAIStatus(AActor* BossActor);

    void ToggleHelpText();

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

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UAIStatusWidget> BossAIStatusWidget;

    /* Else */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHUDHelpWidget> HelpWidget;
};