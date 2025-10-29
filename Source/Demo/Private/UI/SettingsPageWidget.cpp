// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SettingsPageWidget.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "Settings/DemoUserSettings.h"
#include "UI/Settings/SettingsSectionWidget.h"

void USettingsPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(GraphicsTabButton && GraphicsPageWidget
        && AudioTabButton && AudioPageWidget,
        TEXT("Failed to bind widgets."));

    bUseTabButtonImages = false;

    // Setup tab entries
    TabEntries.Emplace(DemoGameplayTags::UI_Settings_Graphics, GraphicsTabButton, nullptr, GraphicsPageWidget);
    TabEntries.Emplace(DemoGameplayTags::UI_Settings_Audio, AudioTabButton, nullptr, AudioPageWidget);
    InitTabMenu();
}

void USettingsPageWidget::SyncUIWithUserSettings()
{
    for (FTabEntry& TabEntry : TabEntries)
    {
        if (USettingsSectionWidget* SettingsSectionWidget = Cast<USettingsSectionWidget>(TabEntry.Widget))
        {
            SettingsSectionWidget->SyncUIWithUserSettings();
        }
    }
}