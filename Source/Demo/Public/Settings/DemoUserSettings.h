// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Framework/Commands/InputChord.h"
#include "GameplayTagContainer.h"
#include "DemoUserSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBoolSettingChanged, FGameplayTag /* InTag */, bool /* bNewValue */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFloatSettingChanged, FGameplayTag /* InTag */, float /* NewValue */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInputKeySettingChanged, FGameplayTag /* InTag */, const FInputChord& /* NewChord */);

/**
 * GameUserSettings subclass for Demo project.
 *
 * If there are too many getters and setters, consider combining related settings.
 * e.g. GetControlsBoolSetting(FGameplayTag InTag)
 */
UCLASS()
class DEMO_API UDemoUserSettings : public UGameUserSettings
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Statics
    ////////////////////////////////////////////////////////
public:
    static UDemoUserSettings* GetDemoUserSettings()
    {
        return Cast<UDemoUserSettings>(GEngine ? GEngine->GetGameUserSettings() : nullptr);
    }

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnBoolSettingChanged OnAudioBoolSettingChanged;
    FOnFloatSettingChanged OnAudioFloatSettingChanged;
    FOnBoolSettingChanged OnControlsBoolSettingChanged;
    FOnInputKeySettingChanged OnInputKeySettingChanged;

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    virtual void SetToDefaults() override;

    ////////////////////////////////////////////////////////
    //        Functions - Audio
    ////////////////////////////////////////////////////////
public:
    // 0.f to 1.f
    // @return nullptr if not found
    const float* GetVolumeSetting(FGameplayTag InCategory) const { return VolumeSettings.Find(InCategory); }

    // 0.f to 1.f
    void SetVolumeSetting(FGameplayTag InCategory, float InVolume);

    bool GetMuteWhenUnfocused() const { return bMuteWhenUnfocused; }

    void SetMuteWhenUnfocused(bool bInMuteWhenUnfocused);

    ////////////////////////////////////////////////////////
    //        Functions - Controls
    ////////////////////////////////////////////////////////
public:
    bool GetWalkInputToggle() const { return bWalkInputToggle; }

    void SetWalkInputToggle(bool bInWalkInputToggle);

    bool GetSprintInputToggle() const { return bSprintInputToggle; }

    void SetSprintInputToggle(bool bInSprintInputToggle);

    const TMap<FGameplayTag, FInputChord>& GetInputKeyMap() const { return InputKeySettings; }

    // @return nullptr if not found
    const FInputChord* GetInputKey(FGameplayTag InTag) const { return InputKeySettings.Find(InTag); }

    void SetInputKey(FGameplayTag InTag, const FInputChord& InChord);

    ////////////////////////////////////////////////////////
    //        Variables - Audio
    ////////////////////////////////////////////////////////
private:
    /* Volume Settings */
    // {AudioCategory, Volume}
    UPROPERTY(Config)
    TMap<FGameplayTag, float> VolumeSettings;

    /* Else */
    UPROPERTY(Config)
    bool bMuteWhenUnfocused;

    ////////////////////////////////////////////////////////
    //        Variables - Controls
    ////////////////////////////////////////////////////////
private:
    /* Basics */
    // true = toggle, false = hold
    UPROPERTY(Config)
    bool bWalkInputToggle;

    // true = toggle, false = hold
    UPROPERTY(Config)
    bool bSprintInputToggle;

    /* Key bindings */
    // {InputTag, InputChord}
    UPROPERTY(Config)
    TMap<FGameplayTag, FInputChord> InputKeySettings;
};