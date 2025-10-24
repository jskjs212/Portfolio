// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "TabMenuWidget.generated.h"

class ADemoPlayerController;
class UImage;
class USoundBase;
class UTabButton;
class UWidgetSwitcher;

/**
 * Struct that ties together a tab's widgets.
 */
USTRUCT()
struct FTabEntry
{
    GENERATED_BODY()

    FGameplayTag Tag;

    TObjectPtr<UTabButton> TabButton;

    TObjectPtr<UImage> Image;

    TObjectPtr<UWidget> Widget;
};

/**
 * Tab Menu Widget
 * Base class for menu widgets that have multiple tab (page) widgets.
 * i.e. PlayerMenu, TraderMenu, Inventory, etc.
 * Child class should call InitTabMenu() in OnInitialized().
 * bUseTabButtonImages: Three colors will be used for images if true, otherwise (false) for buttons.
 */
UCLASS()
class DEMO_API UTabMenuWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    // Set visibility and focus to the active tab.
    void SetVisibilityAndFocus(bool bShow);

    void SelectTab(int32 PageIndex);

    void SelectTab(FGameplayTag InTag);

protected:
    // Called after setting up TabEntries in child class.
    // Bind tab button events and select the first tab.
    void InitTabMenu();

    virtual void SetFocusToWidget(UWidget* InWidget);

    void UpdateTabButtonColor(FTabEntry& InTabEntry, bool bActive, bool bHovered);

    void CancelDragDrop();

    ADemoPlayerController* GetDemoPlayerController();

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
protected:
    void HandleTabButtonClicked(FGameplayTag InTag);

    void HandleTabButtonHovered(FGameplayTag InTag);

    void HandleTabButtonUnhovered(FGameplayTag InTag);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> PageSwitcher;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    TArray<FTabEntry> TabEntries;

    bool bUseTabButtonImages{false};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FLinearColor TabButtonActiveColor;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FLinearColor TabButtonInactiveColor;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FLinearColor TabButtonHoveredColor;

    // @TODO - Audio
    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<USoundBase> TabButtonHoveredSound;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<USoundBase> TabButtonClickSound;

private:
    FGameplayTag ActiveTabTag;

    TWeakObjectPtr<ADemoPlayerController> CachedDemoPlayerController;
};