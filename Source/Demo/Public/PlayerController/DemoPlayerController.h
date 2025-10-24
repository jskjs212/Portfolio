// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DemoTypes/DemoTypes.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "UI/ItemActionDispatcher.h"
#include "DemoPlayerController.generated.h"

class IInteractable;
class UContextMenuWidget;
class UCursorWidget;
class UPlayerMenuWidget;
class USoundBase;

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
        InitCursor();
    }

    // Show or hide player menu.
    // @param TabTag: If valid, switch to the tab with the tag.
    void ShowPlayerMenu(bool bShow, FGameplayTag TabTag = FGameplayTag::EmptyTag);

    void ToggleHelpText();

    void SetCursorState(ECursorState NewCursorState);

private:
    void InitDemoHUD();

    void InitPlayerMenu();

    void InitCursor();

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
private:
    TSubclassOf<UPlayerMenuWidget> PlayerMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UPlayerMenuWidget> PlayerMenuWidget;

    TSubclassOf<UCursorWidget> CursorWidgetClass;

    UPROPERTY()
    TObjectPtr<UCursorWidget> CursorWidget;

    // @TODO - Audio
    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<USoundBase> PlayerMenuOpenSound;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<USoundBase> PlayerMenuCloseSound;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    ECursorState CursorState{ECursorState::Default};
};