// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/DemoUserSettings.h"
#include "Audio/DemoSoundTags.h"
#include "DemoTypes/LogCategories.h"

UDemoUserSettings::UDemoUserSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
    VolumeSettings{
        {DemoSoundTags::Master, &MasterVolume},
        {DemoSoundTags::Music, &MusicVolume},
        {DemoSoundTags::SFX, &SFXVolume},
        {DemoSoundTags::UI, &UIVolume},
        {DemoSoundTags::Voice, &VoiceVolume}
    }
{
}

void UDemoUserSettings::SetToDefaults()
{
    Super::SetToDefaults();

    // Initialize volume settings
    for (auto& [Category, VolumePtr] : VolumeSettings)
    {
        *VolumePtr = 1.f;
    }
}

float UDemoUserSettings::GetVolumeSetting(FGameplayTag InCategory) const
{
    for (const auto& [Category, VolumePtr] : VolumeSettings)
    {
        if (Category == InCategory)
        {
            return *VolumePtr;
        }
    }
    return -1.f;
}

void UDemoUserSettings::SetVolumeSetting(FGameplayTag InCategory, float InVolume)
{
    for (auto& [Category, VolumePtr] : VolumeSettings)
    {
        if (Category == InCategory)
        {
            *VolumePtr = FMath::Clamp(InVolume, 0.f, 1.f);
            OnVolumeSettingChanged.ExecuteIfBound(InCategory, *VolumePtr);
            UE_LOG(LogSettings, Warning, TEXT("@TEST - Volume set. Category: %s, Volume: %f"), *InCategory.ToString(), *VolumePtr);
            return;
        }
    }
}