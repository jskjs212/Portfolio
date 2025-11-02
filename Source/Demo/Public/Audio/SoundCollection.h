// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SoundCollection.generated.h"

class USoundBase;
class USoundClass;
class USoundMix;

/**
 * Sound with the category itself as the key will be played as the sample sound at the settings.
 * e.g. {Sound.SFX, SFXSample} (not for Music)
 * 
 * @TODO - Configure each map in a new DataAsset class for convenience? or Plugin that helps editing maps?
 */
UCLASS()
class DEMO_API USoundCollection : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Audio|Class", meta = (Categories = "Sound"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundClass>> SoundClassMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio|Class")
    TSoftObjectPtr<USoundMix> GlobalSoundMix;

    UPROPERTY(EditDefaultsOnly, Category = "Audio|Sound", meta = (Categories = "Sound.Music"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> MusicMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio|Sound", meta = (Categories = "Sound.SFX"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> SFXMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio|Sound", meta = (Categories = "Sound.UI"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> UIMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio|Sound", meta = (Categories = "Sound.Voice"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> VoiceMap;
};