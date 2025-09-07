// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TabMenuWidget.h"
#include "InventoryPageWidget.generated.h"

class UTabButton;
class UWrapBox;

/**
 *
 */
UCLASS()
class DEMO_API UInventoryPageWidget : public UTabMenuWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
    virtual void ShowMenu(bool bShow) override;

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    /* Weapon */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> WeaponTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> WeaponWrapBox;

    /* Armor */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> ArmorTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> ArmorWrapBox;

    /* Consumable */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> ConsumableTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> ConsumableWrapBox;
};