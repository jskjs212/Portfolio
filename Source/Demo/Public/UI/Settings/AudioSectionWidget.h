// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Settings/SettingsSectionWidget.h"
#include "GameplayTagContainer.h"
#include "AudioSectionWidget.generated.h"

class UCheckBox;
class UTagSlider;
class UTextBlock;

struct FVolumeSettingsEntry
{
    FGameplayTag Category;
    TObjectPtr<UTagSlider> Slider;
    TObjectPtr<UTextBlock> Text;
};

/**
 *
 */
UCLASS()
class DEMO_API UAudioSectionWidget : public USettingsSectionWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    virtual void SyncUIWithUserSettings() override;

private:
    void HandleTagSliderMouseCaptureEnd(FGameplayTag InTag);

    void HandleTagSliderValueChanged(FGameplayTag InTag, float InValue);

    // Only MuteWhenUnfocusedCheckBox uses this.
    UFUNCTION()
    void HandleCheckBoxStateChanged(bool bIsChecked);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    /* Volume Settings */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagSlider> MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MasterVolumeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagSlider> MusicVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MusicVolumeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagSlider> SFXVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> SFXVolumeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagSlider> UIVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> UIVolumeText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagSlider> VoiceVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> VoiceVolumeText;

    /* Else */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCheckBox> MuteWhenUnfocusedCheckBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MuteWhenUnfocusedStateText;

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    static constexpr inline float VolumeMaxValue{100.f};
    static constexpr inline float VolumeStepSize{5.f};

    TArray<FVolumeSettingsEntry> VolumeSettingsEntries;
};