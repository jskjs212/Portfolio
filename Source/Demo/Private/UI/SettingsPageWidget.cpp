// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SettingsPageWidget.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "UI/Settings/SettingsSectionWidget.h"

void USettingsPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(GraphicsTabButton && GraphicsSectionWidget
        && AudioTabButton && AudioSectionWidget
        && ControlsTabButton && ControlsSectionWidget,
        TEXT("Failed to bind widgets."));

    bUseTabButtonImages = false;

    // Setup tab entries
    TabEntries.Emplace(DemoGameplayTags::Setting_Graphics, GraphicsTabButton, nullptr, GraphicsSectionWidget);
    TabEntries.Emplace(DemoGameplayTags::Setting_Audio, AudioTabButton, nullptr, AudioSectionWidget);
    TabEntries.Emplace(DemoGameplayTags::Setting_Controls, ControlsTabButton, nullptr, ControlsSectionWidget);
    InitTabMenu();
}