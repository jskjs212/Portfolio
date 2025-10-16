// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "UI/ItemActionDispatcher.h"
#include "DemoPlayerController.generated.h"

class IInteractable;
class UContextMenuWidget;
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
    }

    // Show or hide player menu.
    // @param TabTag: If valid, switch to the tab with the tag.
    void ShowPlayerMenu(bool bShow, FGameplayTag TabTag = FGameplayTag::EmptyTag);

    void ToggleHelpText();

private:
    void InitDemoHUD();

    void InitPlayerMenu();

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

    // @TODO - Audio
    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<USoundBase> PlayerMenuOpenSound;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<USoundBase> PlayerMenuCloseSound;
};