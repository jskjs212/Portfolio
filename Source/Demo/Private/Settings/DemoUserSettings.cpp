// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/DemoUserSettings.h"
#include "Audio/DemoSoundTags.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "Settings/DemoProjectSettings.h"

UDemoUserSettings::UDemoUserSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
    CategoryDataArray{
        {DemoSoundTags::Master, &MasterVolume},
        {DemoSoundTags::Music,  &MusicVolume},
        {DemoSoundTags::SFX,    &SFXVolume},
        {DemoSoundTags::UI,     &UIVolume},
        {DemoSoundTags::Voice,  &VoiceVolume}
    }
{
}

void UDemoUserSettings::SetToDefaults()
{
    Super::SetToDefaults();

    /* Audio */
    for (FAudioCategoryVolumeData& CategoryData : CategoryDataArray)
    {
        *CategoryData.VolumePtr = 1.f;
    }
    bMuteWhenUnfocused = true;
}

float UDemoUserSettings::GetVolumeSetting(FGameplayTag InCategory) const
{
    for (const FAudioCategoryVolumeData& CategoryData : CategoryDataArray)
    {
        if (CategoryData.Category == InCategory)
        {
            return *CategoryData.VolumePtr;
        }
    }
    return -1.f;
}

void UDemoUserSettings::SetVolumeSetting(const UObject* WorldContextObject, FGameplayTag InCategory, float InVolume)
{
    for (FAudioCategoryVolumeData& CategoryData : CategoryDataArray)
    {
        if (CategoryData.Category == InCategory)
        {
            *CategoryData.VolumePtr = FMath::Clamp(InVolume, 0.f, 1.f);

            // Broadcasting (AudioCategory, NewVolume)
            OnFloatSettingChanged.Broadcast(InCategory, *CategoryData.VolumePtr);
            return;
        }
    }
}

void UDemoUserSettings::SetMuteWhenUnfocused(bool bInMuteWhenUnfocused)
{
    bMuteWhenUnfocused = bInMuteWhenUnfocused;
    OnBoolSettingChanged.Broadcast(DemoGameplayTags::Settings_Audio_MuteWhenUnfocused, bMuteWhenUnfocused);
}