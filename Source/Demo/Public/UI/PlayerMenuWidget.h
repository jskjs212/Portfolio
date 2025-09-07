// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TabMenuWidget.h"
#include "PlayerMenuWidget.generated.h"

class UButton;
class UInventoryPageWidget;
class UStatsPageWidget;
class UTabButton;
class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UPlayerMenuWidget : public UTabMenuWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    // Handle before child widgets.
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    // Handle after child widgets.
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
    virtual void ShowMenu(bool bShow) override;

    UFUNCTION()
    void HideMenu() { ShowMenu(false); }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    /* Stats */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> StatsTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UStatsPageWidget> StatsPageWidget;

    /* Inventory */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> InventoryTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UInventoryPageWidget> InventoryPageWidget;

    /* Else */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CloseButton;
};