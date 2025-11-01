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
class USoundClass;
class USoundMix;
class USoundCollection;
class USoundConcurrency;

USTRUCT()
struct FAudioCategorySoundData
{
    GENERATED_BODY()

    FGameplayTag Category;

    UPROPERTY()
    TObjectPtr<USoundClass> SoundClass;

    const TMap<FGameplayTag, TSoftObjectPtr<USoundBase>>* Map;
};

struct FSoundQueryResult
{
    USoundBase* Sound{nullptr};

    FGameplayTag Category;
};

/**
 * Demo audio subsystem that plays sounds based on GameplayTags.
 *
 * Categories: Music, SFX, UI, Voice
 * Music: Background music
 * SFX: Sound Effects (paused during pause)
 * UI: UI sounds, notifications (not paused)
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
    //        Subsystem functions
    ////////////////////////////////////////////////////////
public:
    UDemoAudioSubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    ////////////////////////////////////////////////////////
    //        Audio functions
    ////////////////////////////////////////////////////////
public:
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

    // Load user settings and bind to changes.
    void LoadUserAudioSettings();

private:
    void InitAudioData();

    void ClearAudioComponent();

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    void HandleWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

    void HandlePostLoadMapWithWorld(UWorld* LoadedWorld);

    void HandleAudioBoolUserSettingChanged(FGameplayTag InTag, bool bNewValue);

    void HandleAudioFloatUserSettingChanged(FGameplayTag InTag, float NewValue);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
private:
    // @return Data is not valid if Sound is nullptr.
    FSoundQueryResult GetSoundByTag(FGameplayTag SoundTag);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    bool bHasBoundToUserSettings{false};

    FGameplayTag DefaultMusicTag;

    FDelegateHandle WorldCleanupHandle;

    UPROPERTY()
    TObjectPtr<USoundMix> GlobalSoundMix;

    // {Category, SoundClass, MapPtr}
    UPROPERTY()
    TArray<FAudioCategorySoundData> CategoryDataArray;

    // Keep the loaded DataAsset alive.
    UPROPERTY(Transient)
    TObjectPtr<const USoundCollection> SoundCollection;
};