// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoHUDWidget.generated.h"

class IInteractable;
class UInteractPromptWidget;
class UItemInfoWidget;

/**
 * Demo HUD Widget
 * ResourceBar (Health, Stamina), Interact (ItemInfo, ButtonGuide?), Crosshair
 */
UCLASS()
class DEMO_API UDemoHUDWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    void NativeOnInitialized() override;

    // Update interact widgets.
    void UpdateInteractWidgets(IInteractable* Interactable);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemInfoWidget> ItemInfoWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UInteractPromptWidget> InteractPromptWidget;
};