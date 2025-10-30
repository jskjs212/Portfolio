// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "GameplayTagContainer.h"
#include "DemoUserSettings.generated.h"

struct FAudioCategoryVolumeData
{
    FGameplayTag Category;

    float* VolumePtr{nullptr};
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBoolSettingChanged, FGameplayTag /* InTag */, bool /* NewValue */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFloatSettingChanged, FGameplayTag /* InTag */, float /* NewValue */);

/**
 * GameUserSettings subclass for Demo project.
 * If there are too many listeners to delegates, split into multiple delegates for graphics, audio, etc.
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
    FOnBoolSettingChanged OnBoolSettingChanged;
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
    void SetVolumeSetting(const UObject* WorldContextObject, FGameplayTag InCategory, float InVolume);

    bool GetMuteWhenUnfocused() const { return bMuteWhenUnfocused; }

    void SetMuteWhenUnfocused(bool bInMuteWhenUnfocused);

    ////////////////////////////////////////////////////////
    //        Variables - Config
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

    /* Else */
    UPROPERTY(Config)
    bool bMuteWhenUnfocused;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    // {Category, VolumePtr}
    TArray<FAudioCategoryVolumeData> CategoryDataArray;
};