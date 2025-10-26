// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "DemoAudioSubsystem.generated.h"

class USoundBase;

USTRUCT()
struct FAudioCategoryData
{
    GENERATED_BODY()

    float Volume{1.f};

    const TMap<FGameplayTag, TSoftObjectPtr<USoundBase>>* Map;
};

/**
 * Categories: Music, SFX, UI, Voice
 * Music: Background musics
 * SFX: Sound Effects
 * UI: UI sounds, notifications
 * Voice: Character voices, dialogues
 *
 * Limited interface parameters for now. Expand if needed.
 * e.g. StartTime, Concurrency, Attenuation, bIsUISound, FadeIn, FadeOut, etc.
 *
 * Consider adding: Array of {MusicTag, Priority} for handling current background music.
 */
UCLASS()
class DEMO_API UDemoAudioSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag, float VolumeMultiplier = 1.f) const;

    // @param Location: World position to play sound at
    // @param Rotation: World rotation to play sound at
    void PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, FRotator Rotation, float VolumeMultiplier = 1.f) const;

    void PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, float VolumeMultiplier = 1.f) const
    {
        PlaySoundAtLocation(WorldContextObject, SoundTag, Location, FRotator::ZeroRotator, VolumeMultiplier);
    }
    // @TODO - Handle BGM according to game state.
    void PlayDefaultBGM(const UObject* WorldContextObject) const;

    // @TODO - SpawnSoundAttached

private:
    // @return {Sound, CategoryVolume}
    TPair<USoundBase*, float> GetSoundByTag(FGameplayTag SoundTag) const;

    ////////////////////////////////////////////////////////
    //        Variables - Core
    ////////////////////////////////////////////////////////
private:
    float MasterVolume{1.f};

    // {AudioCategory, AudioCategoryData}
    TMap<FGameplayTag, FAudioCategoryData> AudioMap;
};