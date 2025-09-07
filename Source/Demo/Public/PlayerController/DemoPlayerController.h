// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DemoPlayerController.generated.h"

class IInteractable;
class UPlayerMenuWidget;

/**
 *
 */
UCLASS()
class DEMO_API ADemoPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ADemoPlayerController();

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    void InitUI()
    {
        InitDemoHUD();
        InitPlayerMenu();
    }

    void ShowPlayerMenu(bool bShow);

private:
    void InitDemoHUD();

    void InitPlayerMenu();

private:
    // Show or hide interact widget.
    // HUD widget will decide what to show based on actual type (open, pickup, etc.).
    void HandleInteractableFocused(IInteractable* NewFocusedInteractable);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    TSubclassOf<UPlayerMenuWidget> PlayerMenuWidgetClass;

    TObjectPtr<UPlayerMenuWidget> PlayerMenuWidget;
};