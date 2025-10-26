// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "SoundCollection.generated.h"

class USoundBase;

/**
 *
 */
UCLASS()
class DEMO_API USoundCollection : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Audio", meta = (Categories = "Sound.Music"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> MusicMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio", meta = (Categories = "Sound.SFX"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> SFXMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio", meta = (Categories = "Sound.UI"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> UIMap;

    UPROPERTY(EditDefaultsOnly, Category = "Audio", meta = (Categories = "Sound.Voice"))
    TMap<FGameplayTag, TSoftObjectPtr<USoundBase>> VoiceMap;
};