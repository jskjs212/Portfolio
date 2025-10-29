// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "GameplayTagContainer.h"
#include "DemoUserSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFloatSettingChanged, FGameplayTag /* InCategory */, float /* InVolume */);

/**
 * GameUserSettings subclass for Demo project.
 * If there are too many listners to delegates,
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
    FOnFloatSettingChanged OnFloatSettingChanged;

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    UDemoUserSettings(const FObjectInitializer& ObjectInitializer);

    virtual void SetToDefaults() override;

    ////////////////////////////////////////////////////////
    //        Functions - Audio
    ////////////////////////////////////////////////////////
public:
    // 0.f to 1.f
    // @return -1.f if InCategory is not valid.
    float GetVolumeSetting(FGameplayTag InCategory) const;

    // 0.f to 1.f
    void SetVolumeSetting(FGameplayTag InCategory, float InVolume);

    bool GetMuteWhenUnfocused() const { return bMuteWhenUnfocused; }

    void SetMuteWhenUnfocused(bool bInMuteWhenUnfocused);

    ////////////////////////////////////////////////////////
    //        Variables - Audio
    ////////////////////////////////////////////////////////
private:
    /* Volume Settings */
    UPROPERTY(Config)
    float MasterVolume;

    UPROPERTY(Config)
    float MusicVolume;

    UPROPERTY(Config)
    float SFXVolume;

    UPROPERTY(Config)
    float UIVolume;

    UPROPERTY(Config)
    float VoiceVolume;

    TArray<TPair<FGameplayTag, float*>> VolumeSettings;

    /* Else */
    UPROPERTY(Config)
    bool bMuteWhenUnfocused;
};