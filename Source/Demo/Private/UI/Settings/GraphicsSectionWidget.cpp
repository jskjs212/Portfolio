// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Settings/GraphicsSectionWidget.h"
#include "Components/ComboBoxString.h"
#include "DemoTypes/LogCategories.h"
#include "Settings/DemoUserSettings.h"

void UGraphicsSectionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(FPSLimitComboBox, TEXT("Failed to bind widgets."));

    FPSLimitComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleFPSLimitChanged);
}

void UGraphicsSectionWidget::SyncUIWithUserSettings()
{
    // Sync UI elements with UserSettings values
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        // FPS Limit
        const int32 CurrentFPSLimit = static_cast<int32>(UserSettings->GetFrameRateLimit());
        const FString CurrentFPSLimitStr = CurrentFPSLimit == 0 ? UnlimitedOption : FString::FromInt(CurrentFPSLimit);
        const auto OptionIndex = FPSLimitComboBox->FindOptionIndex(CurrentFPSLimitStr);
        if (OptionIndex == INDEX_NONE)
        {
            FPSLimitComboBox->SetSelectedOption(UnlimitedOption);
        }
        else
        {
            FPSLimitComboBox->SetSelectedIndex(OptionIndex);
        }
    }
}

void UGraphicsSectionWidget::HandleFPSLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{

    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        const float NewFPSLimit = SelectedItem == UnlimitedOption ? 0.f : FCString::Atof(*SelectedItem);
        UserSettings->SetFrameRateLimit(NewFPSLimit);
        UserSettings->ApplySettings(false);
        UE_LOG(LogSettings, Display, TEXT("FPS Limit set: %f"), NewFPSLimit);
    }
}