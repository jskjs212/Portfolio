// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Items/ItemTypes.h"
#include "UI/ItemActionDispatcher.h"
#include "DemoPlayerController.generated.h"

class IInteractable;
class UContextMenuWidget;
class UPlayerMenuWidget;

/**
 *
 */
UCLASS()
class DEMO_API ADemoPlayerController : public APlayerController
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
private:
    UPROPERTY()
    TObjectPtr<UItemActionDispatcher> ItemActionDispatcher;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    ADemoPlayerController();

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
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    UItemActionDispatcher* GetItemActionDispatcher() const { return ItemActionDispatcher; }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    TSubclassOf<UPlayerMenuWidget> PlayerMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UPlayerMenuWidget> PlayerMenuWidget;
};