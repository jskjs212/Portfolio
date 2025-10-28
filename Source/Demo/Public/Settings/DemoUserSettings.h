// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "GameplayTagContainer.h"
#include "DemoUserSettings.generated.h"

DECLARE_DELEGATE_TwoParams(FOnVolumeSettingChanged, FGameplayTag /* InCategory */, float /* InVolume */);

/**
 *
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
    FOnVolumeSettingChanged OnVolumeSettingChanged;

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    UDemoUserSettings(const FObjectInitializer& ObjectInitializer);

    virtual void SetToDefaults() override;

    // 0.f to 1.f
    // @return -1.f if InCategory is not valid.
    float GetVolumeSetting(FGameplayTag InCategory) const;

    // 0.f to 1.f
    void SetVolumeSetting(FGameplayTag InCategory, float InVolume);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
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
};