// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/DemoUserSettings.h"
#include "Audio/DemoSoundTags.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "Settings/DemoProjectSettings.h"

void UDemoUserSettings::SetToDefaults()
{
    Super::SetToDefaults();

    /* Audio */
    VolumeSettings = TMap<FGameplayTag, float>{
        {DemoSoundTags::Master, 1.f},
        {DemoSoundTags::Music,  1.f},
        {DemoSoundTags::SFX,    1.f},
        {DemoSoundTags::UI,     1.f},
        {DemoSoundTags::Voice,  1.f}
    };
    bMuteWhenUnfocused = true;

    /* Controls */
    bWalkInputToggle = true;
    bSprintInputToggle = false;
    InputKeySettings = {
        {DemoGameplayTags::Input_Jump,  FInputChord{EKeys::F}},
        {DemoGameplayTags::Input_Dodge, FInputChord{EKeys::SpaceBar}}
    };
}

void UDemoUserSettings::SetVolumeSetting(FGameplayTag InCategory, float InVolume)
{
    float* VolumePtr = VolumeSettings.Find(InCategory);
    if (VolumePtr)
    {
        *VolumePtr = FMath::Clamp(InVolume, 0.f, 1.f);
        OnAudioFloatSettingChanged.Broadcast(InCategory, *VolumePtr);
    }
    else
    {
        DemoLOG_CF(LogAudio, Warning, TEXT("Invalid tag: %s"), *InCategory.ToString());
    }
}

void UDemoUserSettings::SetMuteWhenUnfocused(bool bInMuteWhenUnfocused)
{
    if (bMuteWhenUnfocused != bInMuteWhenUnfocused)
    {
        bMuteWhenUnfocused = bInMuteWhenUnfocused;
        OnAudioBoolSettingChanged.Broadcast(DemoGameplayTags::Setting_Audio_MuteWhenUnfocused, bMuteWhenUnfocused);
    }
}

void UDemoUserSettings::SetWalkInputToggle(bool bInWalkInputToggle)
{
    if (bWalkInputToggle != bInWalkInputToggle)
    {
        bWalkInputToggle = bInWalkInputToggle;
        OnControlsBoolSettingChanged.Broadcast(DemoGameplayTags::Setting_Controls_WalkInputToggle, bWalkInputToggle);
    }
}

void UDemoUserSettings::SetSprintInputToggle(bool bInSprintInputToggle)
{
    if (bSprintInputToggle != bInSprintInputToggle)
    {
        bSprintInputToggle = bInSprintInputToggle;
        OnControlsBoolSettingChanged.Broadcast(DemoGameplayTags::Setting_Controls_SprintInputToggle, bSprintInputToggle);
    }
}

void UDemoUserSettings::SetInputKey(FGameplayTag InTag, const FInputChord& InChord)
{
    if (FInputChord* ExistingChord = InputKeySettings.Find(InTag))
    {
        if (*ExistingChord != InChord)
        {
            *ExistingChord = InChord;
            OnInputKeySettingChanged.Broadcast(InTag, InChord);
        }
    }
    else // New element
    {
        InputKeySettings.Emplace(InTag, InChord);
        OnInputKeySettingChanged.Broadcast(InTag, InChord);
    }
}