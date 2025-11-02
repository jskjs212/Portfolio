// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TabMenuWidget.h"
#include "SettingsPageWidget.generated.h"

class UDemoUserSettings;
class USettingsSectionWidget;
class UTagButton;

/**
 * Settings Page Widget
 * Each options section is hardcoded for now.
 * In future, make it data-driven + generic handler.
 * - Entry: Key, Type (Slider, etc.), DefaultValue, UWidget
 * - Section: List of Entries, SetupBindings()
 * - Changed event -> Handle_WidgetType_Changed -> HandleValueChanged_ValueType
 */
UCLASS()
class DEMO_API USettingsPageWidget : public UTabMenuWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    /* Graphics */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagButton> GraphicsTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USettingsSectionWidget> GraphicsSectionWidget;

    /* Audio */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagButton> AudioTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USettingsSectionWidget> AudioSectionWidget;

    /* Controls */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagButton> ControlsTabButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USettingsSectionWidget> ControlsSectionWidget;
};