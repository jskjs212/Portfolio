// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "DemoAudioSubsystem.generated.h"

class UAudioComponent;
class UInitialActiveSoundParams;
class USoundAttenuation;
class USoundBase;
class USoundCollection;
class USoundConcurrency;

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
 * @TODO - SpawnSound with AudioComponent pooling?
 *
 * Consider adding:
 * - Parameters: StartTime, FadeIn, FadeOut, Concurrency, Attenuation, etc.
 * - Array of {MusicTag, Priority} for handling current background music state.
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

    // Play a non-looping sound directly.
    void PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundConcurrency* ConcurrencySettings = nullptr, const AActor* OwningActor = nullptr);

    // Play a non-looping sound at a given location.
    // @param Location: World position to play sound at
    // @param Rotation: World rotation to play sound at
    void PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, FRotator Rotation, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, const AActor* OwningActor = nullptr, const UInitialActiveSoundParams* InitialParams = nullptr);

    // Play a non-looping sound at a given location.
    void PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, const AActor* OwningActor = nullptr, const UInitialActiveSoundParams* InitialParams = nullptr)
    {
        PlaySoundAtLocation(WorldContextObject, SoundTag, Location, FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, OwningActor, InitialParams);
    }

    // Play a non-looping sound attached to a given component.
    void PlaySoundAttached(FGameplayTag SoundTag, USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = true, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true);

    // Play a non-looping sound attached to a given component.
    void PlaySoundAttached(FGameplayTag SoundTag, USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector::ZeroVector, EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset, bool bStopWhenAttachedToDestroyed = true, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, bool bAutoDestroy = true)
    {
        PlaySoundAttached(SoundTag, AttachToComponent, AttachPointName, Location, FRotator::ZeroRotator, LocationType, bStopWhenAttachedToDestroyed, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, bAutoDestroy);
    }

    // Play music (BGM)
    void PlayMusic(const UObject* WorldContextObject, FGameplayTag MusicTag, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f);

    // Play default music (BGM)
    void PlayDefaultMusic(const UObject* WorldContextObject, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f)
    {
        PlayMusic(WorldContextObject, DefaultMusicTag, VolumeMultiplier, PitchMultiplier);
    }

private:
    void InitAudioData();

    void LoadUserAudioSettings();

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