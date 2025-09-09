// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "TabMenuWidget.generated.h"

class UImage;
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
 * Child class should call InitMenu() in OnInitialized().
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

protected:
    // Called after setting up TabEntries in child class.
    // Bind tab button events and select the first tab.
    void InitMenu();

    void SelectTab(int32 PageIndex);

    void SelectTab(FGameplayTag InTag);

    virtual void SetFocusToWidget(UWidget* InWidget);

    void UpdateTabButtonColor(FTabEntry& InTabEntry, const FLinearColor& InColor);

    /* Tab button event handlers */
    FORCEINLINE void HandleTabButtonClicked(FGameplayTag InTag) { SelectTab(InTag); }

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

    UPROPERTY(EditAnywhere, Category = "Appearance|Tab")
    FLinearColor TabButtonActiveColor;

    UPROPERTY(EditAnywhere, Category = "Appearance|Tab")
    FLinearColor TabButtonInactiveColor;

    UPROPERTY(EditAnywhere, Category = "Appearance|Tab")
    FLinearColor TabButtonHoveredColor;

private:
    FGameplayTag ActiveTabTag;
};