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
class UDemoInputConfig;
class UInputMappingContext;
class UPlayerMenuWidget;
class UYouDiedWidget;

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
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    ADemoPlayerController();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        PlayerController functions
    ////////////////////////////////////////////////////////
public:
    void InitUI()
    {
        InitDemoHUD();
        InitPlayerMenu();
        InitCursor();
    }

    // Show or hide player menu.
    // @param TabTag: If valid, switch to the tab with the tag.
    void ShowPlayerMenu(bool bShow, FGameplayTag TabTag = FGameplayTag::EmptyTag);

    // Hide boss AI status if BossActor is nullptr.
    void ShowBossAIStatus(AActor* BossActor);

    void ShowBossVictoryWidget();

    // @return true if BossVictoryWidget was visible and is now hidden.
    void HideBossVictoryWidget();

    // Show "You Died" widget and add after-death input context.
    void ShowYouDiedWidgetAndAddAfterDeathInputContext();

    void ToggleHelpText();

    // End current session and go to main menu.
    void GoToMainMenu();

private:
    void InitDemoHUD();

    void InitPlayerMenu();

    void InitCursor();

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
public:
    void HandleBossDeath();

private:
    void HandleCharacterDeath();

    // Show or hide interact widget.
    // HUD widget will decide what to show based on actual type (open, pickup, etc.).
    void HandleInteractableFocused(IInteractable* NewFocusedInteractable);

    ////////////////////////////////////////////////////////
    //        Input functions
    ////////////////////////////////////////////////////////
private:
    void SetupPlayerInput();

    // ShowPlayerMenu when input action is triggered.
    void ShowPlayerMenuActionStarted();

    // Show PlayerMenu, and select System tab.
    void EscapeActionStarted();

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    UItemActionDispatcher* GetItemActionDispatcher() const { return ItemActionDispatcher; }

    void SetCursorState(ECursorState NewCursorState);

    ////////////////////////////////////////////////////////
    //        Input variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization|Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = "Initialization|Input")
    TObjectPtr<UInputMappingContext> AfterDeathMappingContext;

    UPROPERTY(EditAnywhere, Category = "Initialization|Input")
    TObjectPtr<UDemoInputConfig> PlayerControllerInputConfig;

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    /* Player menu */
    TSubclassOf<UPlayerMenuWidget> PlayerMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UPlayerMenuWidget> PlayerMenuWidget;

    /* Combat */
    TSubclassOf<UUserWidget> BossVictoryWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> BossVictoryWidget;

    TSubclassOf<UUserWidget> YouDiedWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> YouDiedWidget;

    /* Cursor */
    TSubclassOf<UCursorWidget> CursorWidgetClass;

    UPROPERTY()
    TObjectPtr<UCursorWidget> CursorWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Death")
    float YouDiedWidgetDelay{2.5f};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Boss")
    float BossVictoryWidgetDelay{2.f};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Boss")
    float BossVictoryWidgetDuration{3.f};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FName MainMenuLevelName;

    ECursorState CursorState{ECursorState::Default};
};