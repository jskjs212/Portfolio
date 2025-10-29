// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Settings/AudioSectionWidget.h"
#include "Audio/DemoSoundTags.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Settings/DemoUserSettings.h"
#include "UI/Components/TagSlider.h"

void UAudioSectionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(MasterVolumeSlider && MasterVolumeText
        && MusicVolumeSlider && MusicVolumeText
        && SFXVolumeSlider && SFXVolumeText
        && UIVolumeSlider && UIVolumeText
        && VoiceVolumeSlider && VoiceVolumeText
        && MuteWhenUnfocusedCheckBox && MuteWhenUnfocusedStateText,
        TEXT("Failed to bind widgets."));

    // Initialize entries
    VolumeSettingsEntries = {
        {DemoSoundTags::Master, MasterVolumeSlider, MasterVolumeText},
        {DemoSoundTags::Music, MusicVolumeSlider, MusicVolumeText},
        {DemoSoundTags::SFX, SFXVolumeSlider, SFXVolumeText},
        {DemoSoundTags::UI, UIVolumeSlider, UIVolumeText},
        {DemoSoundTags::Voice, VoiceVolumeSlider, VoiceVolumeText}
    };

    // Configure widgets and bind events
    for (FVolumeSettingsEntry& Entry : VolumeSettingsEntries)
    {
        Entry.Slider->MouseUsesStep = true;
        Entry.Slider->SetMinValue(0.f);
        Entry.Slider->SetMaxValue(VolumeMaxValue);
        Entry.Slider->SetStepSize(VolumeStepSize);
        Entry.Slider->SetTypeTag(Entry.Category);
        Entry.Slider->OnTagSliderMouseCaptureEnd.BindUObject(this, &ThisClass::HandleTagSliderMouseCaptureEnd);
        Entry.Slider->OnTagSliderValueChanged.BindUObject(this, &ThisClass::HandleTagSliderValueChanged);
        Entry.Text->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    MuteWhenUnfocusedCheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::HandleCheckBoxStateChanged);
}

void UAudioSectionWidget::SyncUIWithUserSettings()
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        // Volume settings
        for (FVolumeSettingsEntry& Entry : VolumeSettingsEntries)
        {
            const float LoadedVolume = UserSettings->GetVolumeSetting(Entry.Category) * VolumeMaxValue;
            if (LoadedVolume >= 0.f)
            {
                Entry.Slider->SetValue(LoadedVolume);
                Entry.Text->SetText(FText::AsNumber(FMath::RoundToInt(LoadedVolume)));
            }
        }

        // Else
        MuteWhenUnfocusedCheckBox->SetIsChecked(UserSettings->GetMuteWhenUnfocused());
        MuteWhenUnfocusedStateText->SetText(FText::FromString(UserSettings->GetMuteWhenUnfocused() ? TEXT("ON") : TEXT("OFF")));
    }
}

void UAudioSectionWidget::HandleTagSliderMouseCaptureEnd(FGameplayTag InTag)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        // Apply new volume settings
        UserSettings->ApplySettings(false);
    }
}

void UAudioSectionWidget::HandleTagSliderValueChanged(FGameplayTag InTag, float InValue)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        for (const FVolumeSettingsEntry& Entry : VolumeSettingsEntries)
        {
            if (Entry.Category == InTag)
            {
                // Update volume setting and text immediately.
                // Not applying settings until mouse capture ends.
                const float NewVolume = Entry.Slider->GetValue() / VolumeMaxValue;
                UserSettings->SetVolumeSetting(InTag, NewVolume);
                Entry.Text->SetText(FText::AsNumber(FMath::RoundToInt(InValue)));
                break;
            }
        }
    }
}

void UAudioSectionWidget::HandleCheckBoxStateChanged(bool bIsChecked)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        const FString StateStr = bIsChecked ? TEXT("ON") : TEXT("OFF");
        UserSettings->SetMuteWhenUnfocused(bIsChecked);
        MuteWhenUnfocusedStateText->SetText(FText::FromString(StateStr));
        UserSettings->ApplySettings(false);
    }
}