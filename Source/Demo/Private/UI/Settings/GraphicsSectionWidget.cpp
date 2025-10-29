// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Settings/GraphicsSectionWidget.h"
#include "Components/ComboBoxString.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/DemoUserSettings.h"

// Static variable initialization - @hardcoded
const FString UGraphicsSectionWidget::UnlimitedOption{TEXT("Unlimited")};
const TArray<FIntPoint> UGraphicsSectionWidget::Resolutions{
    FIntPoint{1280, 720},
    FIntPoint{1920, 1080},
    FIntPoint{2560, 1440},
    FIntPoint{3840, 2160}
};
const TArray<FString> UGraphicsSectionWidget::QualityOptions{
    TEXT("Low"),
    TEXT("Medium"),
    TEXT("High"),
    TEXT("Epic"),
    TEXT("Cinematic")
};

void UGraphicsSectionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(DisplayModeComboBox && ResolutionComboBox && FPSLimitComboBox && TextureQualityComboBox,
        TEXT("Failed to bind widgets."));

    // Display mode
    DisplayModeComboBox->AddOption(TEXT("Fullscreen"));
    DisplayModeComboBox->AddOption(TEXT("Windowed Fullscreen"));
    DisplayModeComboBox->AddOption(TEXT("Windowed"));
    DisplayModeComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleDisplayModeChanged);

    // Resolution
    TArray<FIntPoint> SupportedResolutions;
    UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);
    for (const FIntPoint& Resolution : Resolutions)
    {
        if (SupportedResolutions.Contains(Resolution))
        {
            // Add only supported resolutions
            const FString ResolutionOption = FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y);
            ResolutionComboBox->AddOption(ResolutionOption);
        }
    }
    ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleResolutionChanged);

    // FPS limit
    // Set default selection to Unlimited
    FPSLimitComboBox->AddOption(TEXT("30"));
    FPSLimitComboBox->AddOption(TEXT("60"));
    FPSLimitComboBox->AddOption(TEXT("90"));
    FPSLimitComboBox->AddOption(TEXT("120"));
    FPSLimitComboBox->AddOption(TEXT("144"));
    FPSLimitComboBox->AddOption(TEXT("240"));
    FPSLimitComboBox->AddOption(UnlimitedOption);
    FPSLimitComboBox->SetSelectedOption(UnlimitedOption);
    FPSLimitComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleFPSLimitChanged);

    // Quality settings
    for (const FString& QualityOption : QualityOptions)
    {
        TextureQualityComboBox->AddOption(QualityOption);
        ShadowQualityComboBox->AddOption(QualityOption);
    }
    TextureQualityComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleTextureQualityChanged);
    ShadowQualityComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::HandleShadowQualityChanged);
}

void UGraphicsSectionWidget::SyncUIWithUserSettings()
{
    // Sync UI elements with UserSettings values
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        // Display mode
        const EWindowMode::Type LoadedDisplayMode = UserSettings->GetFullscreenMode();
        const int32 DisplayModeIndex = EWindowMode::ConvertIntToWindowMode(LoadedDisplayMode);
        DisplayModeComboBox->SetSelectedIndex(DisplayModeIndex);

        // Resolution
        const FIntPoint LoadedResolution = UserSettings->GetScreenResolution();
        const int32 ResolutionIndex = Resolutions.Find(LoadedResolution);
        ResolutionComboBox->SetSelectedIndex(ResolutionIndex);

        // FPS limit
        const int32 LoadedFPSLimit = static_cast<int32>(UserSettings->GetFrameRateLimit());
        if (LoadedFPSLimit == 0)
        {
            // Already set to Unlimited
        }
        else
        {
            const FString FPSLimitStr = FString::FromInt(LoadedFPSLimit);
            const int32 FPSLimitIndex = FPSLimitComboBox->FindOptionIndex(FPSLimitStr);
            if (FPSLimitIndex != INDEX_NONE)
            {
                FPSLimitComboBox->SetSelectedIndex(FPSLimitIndex);
            }
            else
            {
                // If loaded value is not in the options, do nothing.
                // If config was modified manually to an unsupported value, keep that setting until user changes it via UI.
                // But selected option is Unlimited, so it might be confusing.
                DemoLOG_CF(LogSettings, Warning, TEXT("FPS limit (%d) from config file is applied, but (Unlimited) option remains selected in UI."), LoadedFPSLimit);
            }
        }

        // Quality settings
        TextureQualityComboBox->SetSelectedIndex(UserSettings->GetTextureQuality());
        ShadowQualityComboBox->SetSelectedIndex(UserSettings->GetShadowQuality());
    }
}

void UGraphicsSectionWidget::HandleDisplayModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        const int32 SelectedIndex = DisplayModeComboBox->GetSelectedIndex();
        const EWindowMode::Type NewDisplayMode = EWindowMode::ConvertIntToWindowMode(SelectedIndex);
        UserSettings->SetFullscreenMode(NewDisplayMode);
        UserSettings->ConfirmVideoMode();
        UserSettings->ApplySettings(false);

        const bool bDisableResolutionChange = NewDisplayMode == EWindowMode::WindowedFullscreen;
        ResolutionComboBox->SetIsEnabled(!bDisableResolutionChange);
    }
}

void UGraphicsSectionWidget::HandleResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        const int32 SelectedIndex = ResolutionComboBox->GetSelectedIndex();
        const FIntPoint NewResolution = Resolutions.IsValidIndex(SelectedIndex) ? Resolutions[SelectedIndex] : UDemoUserSettings::GetDefaultResolution();
        UserSettings->SetScreenResolution(NewResolution);
        UserSettings->ConfirmVideoMode();
        UserSettings->ApplySettings(false);
    }
}

void UGraphicsSectionWidget::HandleFPSLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        const float NewFPSLimit = SelectedItem == UnlimitedOption ? 0.f : FCString::Atof(*SelectedItem);
        UserSettings->SetFrameRateLimit(NewFPSLimit);
        UserSettings->ApplySettings(false);
    }
}

void UGraphicsSectionWidget::HandleTextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        const int32 SelectedIndex = TextureQualityComboBox->GetSelectedIndex();
        UserSettings->SetTextureQuality(SelectedIndex);
        UserSettings->ApplySettings(false);
    }
}

void UGraphicsSectionWidget::HandleShadowQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        const int32 SelectedIndex = ShadowQualityComboBox->GetSelectedIndex();
        UserSettings->SetShadowQuality(SelectedIndex);
        UserSettings->ApplySettings(false);
    }
}