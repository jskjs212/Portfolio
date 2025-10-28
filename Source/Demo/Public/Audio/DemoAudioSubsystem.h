// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "DemoAudioSubsystem.generated.h"

class UAudioComponent;
class USoundBase;
class USoundCollection;

struct FAudioCategoryData
{
    FGameplayTag Category;

    float Volume{1.f};

    const TMap<FGameplayTag, TSoftObjectPtr<USoundBase>>* Map;
};

struct FSoundQueryResult
{
    USoundBase* Sound{nullptr};

    FGameplayTag Category;

    float CategoryVolume{1.f};
};

/**
 * Categories: Music, SFX, UI, Voice
 * Music: Background music
 * SFX: Sound Effects
 * UI: UI sounds, notifications
 * Voice: Character voices
 *
 * Consider adding:
 * - Parameters: StartTime, FadeIn, FadeOut, Concurrency, Attenuation, etc.
 * - Array of {MusicTag, Priority} for handling current background music.
 * - Attached sounds
 * - Dialogue system
 */
UCLASS()
class DEMO_API UDemoAudioSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
private:
    UPROPERTY()
    TObjectPtr<UAudioComponent> MusicAudioComponent;

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    UDemoAudioSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    void PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag);

    // @param Location: World position to play sound at
    // @param Rotation: World rotation to play sound at
    void PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, FRotator Rotation);

    void PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location)
    {
        PlaySoundAtLocation(WorldContextObject, SoundTag, Location, FRotator::ZeroRotator);
    }

    // Play music (BGM)
    void PlayMusic(const UObject* WorldContextObject, FGameplayTag MusicTag);

    // Play default music (BGM)
    void PlayDefaultMusic(const UObject* WorldContextObject)
    {
        PlayMusic(WorldContextObject, DefaultMusicTag);
    }

private:
    void InitAudioData();

    void ClearAudioComponent();

    void HandleWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

    void HandleVolumeSettingChanged(FGameplayTag InCategory, float InVolume);

    // @return Data is not valid if Sound is nullptr.
    FSoundQueryResult GetSoundByTag(FGameplayTag SoundTag);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    float MasterVolume{1.f};

    FGameplayTag DefaultMusicTag;

    FDelegateHandle WorldCleanupHandle;

    // {Category, Volume, MapPtr}
    TArray<FAudioCategoryData> CategoryDataArray;

    // Keep the loaded DataAsset alive.
    UPROPERTY(Transient)
    TObjectPtr<const USoundCollection> SoundCollection;
};