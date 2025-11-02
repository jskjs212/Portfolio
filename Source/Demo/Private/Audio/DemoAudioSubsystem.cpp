// Fill out your copyright notice in the Description page of Project Settings.

#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Audio/SoundCollection.h"
#include "Components/AudioComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/DemoProjectSettings.h"
#include "Settings/DemoUserSettings.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

UDemoAudioSubsystem::UDemoAudioSubsystem()
{
    DefaultMusicTag = DemoSoundTags::Music_Exploration;
    MusicAudioComponent = nullptr;
}

void UDemoAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitAudioData();
    LoadUserAudioSettings();

    FDelegateHandle Handle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &ThisClass::HandleWorldCleanup);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMapWithWorld);
}

void UDemoAudioSubsystem::Deinitialize()
{
    Super::Deinitialize();

    // Unbind
    if (WorldCleanupHandle.IsValid())
    {
        FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupHandle);
        WorldCleanupHandle.Reset();
    }
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        UserSettings->OnAudioBoolSettingChanged.RemoveAll(this);
        UserSettings->OnAudioFloatSettingChanged.RemoveAll(this);
    }

    ClearAudioComponent();
}

void UDemoAudioSubsystem::PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundConcurrency* ConcurrencySettings, const AActor* OwnerActor)
{
    if (!WorldContextObject)
    {
        return;
    }

    // Query sound
    FSoundQueryResult Result = GetSoundByTag(SoundTag);
    if (!Result.Sound)
    {
        return;
    }
    if (Result.Category == DemoSoundTags::Music)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Use PlayMusic(). Tag: %s"), *SoundTag.ToString());
        return;
    }
    if (Result.Sound->IsLooping())
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Looping sounds not allowed. Tag: %s"), *SoundTag.ToString());
        return;
    }

    const bool bIsUISound = Result.Category == DemoSoundTags::UI;
    UGameplayStatics::PlaySound2D(WorldContextObject, Result.Sound, VolumeMultiplier, PitchMultiplier, StartTime, ConcurrencySettings, OwnerActor, bIsUISound);
}

void UDemoAudioSubsystem::PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings, const AActor* OwningActor, const UInitialActiveSoundParams* InitialParams)
{
    if (!WorldContextObject)
    {
        return;
    }

    // Query sound
    FSoundQueryResult Result = GetSoundByTag(SoundTag);
    if (!Result.Sound)
    {
        return;
    }
    if (Result.Category == DemoSoundTags::Music)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Use PlayMusic(). Tag: %s"), *SoundTag.ToString());
        return;
    }
    if (Result.Sound->IsLooping())
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Looping sounds not allowed. Tag: %s"), *SoundTag.ToString());
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Result.Sound, Location, Rotation, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, OwningActor, InitialParams);
}

void UDemoAudioSubsystem::PlaySoundAttached(FGameplayTag SoundTag, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings, bool bAutoDestroy)
{
    // Query sound
    FSoundQueryResult Result = GetSoundByTag(SoundTag);
    if (!Result.Sound)
    {
        return;
    }
    if (Result.Category == DemoSoundTags::Music)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Use PlayMusic(). Tag: %s"), *SoundTag.ToString());
        return;
    }
    if (Result.Sound->IsLooping())
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Looping sounds not allowed. Tag: %s"), *SoundTag.ToString());
        return;
    }

    UGameplayStatics::SpawnSoundAttached(Result.Sound, AttachToComponent, AttachPointName, Location, Rotation, LocationType, bStopWhenAttachedToDestroyed, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, bAutoDestroy);
}

void UDemoAudioSubsystem::PlayMusic(const UObject* WorldContextObject, FGameplayTag MusicTag, float VolumeMultiplier, float PitchMultiplier)
{
    UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
    if (!World)
    {
        return;
    }

    // Query sound
    FSoundQueryResult Result = GetSoundByTag(MusicTag);
    if (!Result.Sound)
    {
        return;
    }
    if (Result.Category != DemoSoundTags::Music)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Tag is not allowed. Tag: %s"), *MusicTag.ToString());
        return;
    }

    if (MusicAudioComponent)
    {
        // Destroy if the AudioComponent belongs to a different world.
        if (MusicAudioComponent->GetWorld() != World) // _UNLIKELY
        {
            DemoLOG_CF(LogAudio, Error, TEXT("MusicAudioComponent hasn't been cleared in previous world."));
            ClearAudioComponent();
        }
    }

    // Create AudioComponent in this world
    if (!MusicAudioComponent)
    {
        MusicAudioComponent = NewObject<UAudioComponent>(World);
        MusicAudioComponent->bAllowSpatialization = false;
        MusicAudioComponent->SetAutoActivate(false);
        MusicAudioComponent->SetUISound(true);
        MusicAudioComponent->RegisterComponentWithWorld(World);
    }

    // Already playing the same music
    if (MusicAudioComponent->GetSound() == Result.Sound && MusicAudioComponent->IsPlaying())
    {
        return;
    }

    // Play music
    MusicAudioComponent->SetSound(Result.Sound);
    MusicAudioComponent->SetVolumeMultiplier(VolumeMultiplier);
    MusicAudioComponent->SetPitchMultiplier(PitchMultiplier);
    MusicAudioComponent->Play(0.f);
}

void UDemoAudioSubsystem::InitAudioData()
{
    // Load SoundCollection
    const USoundCollection* LoadedSoundCollection = GetDefault<UDemoProjectSettings>()->SoundCollection.LoadSynchronous();
    if (!LoadedSoundCollection)
    {
        DemoLOG_CF(LogSettings, Error, TEXT("Failed to load SoundCollection from DemoProjectSettings."));
        return;
    }
    SoundCollection = LoadedSoundCollection;

    // Initialize audio data
    CategoryDataArray = {
        {DemoSoundTags::Master, nullptr, nullptr},
        {DemoSoundTags::Music,  nullptr, &SoundCollection->MusicMap},
        {DemoSoundTags::SFX,    nullptr, &SoundCollection->SFXMap},
        {DemoSoundTags::UI,     nullptr, &SoundCollection->UIMap},
        {DemoSoundTags::Voice,  nullptr, &SoundCollection->VoiceMap}
    };

    // Load GlobalSoundMix
    GlobalSoundMix = SoundCollection->GlobalSoundMix.LoadSynchronous();
    if (!GlobalSoundMix)
    {
        DemoLOG_CF(LogSettings, Error, TEXT("In SoundCollection, GlobalSoundMix is not set."));
    }

    // Load SoundClasses
    for (FAudioCategorySoundData& CategoryData : CategoryDataArray)
    {
        const TSoftObjectPtr<USoundClass>* SoundClassPtr = SoundCollection->SoundClassMap.Find(CategoryData.Category);
        if (SoundClassPtr)
        {
            CategoryData.SoundClass = SoundClassPtr->LoadSynchronous();
        }
        else
        {
            DemoLOG_CF(LogSettings, Error, TEXT("In SoundCollection, SoundClass is not set for category: %s"), *CategoryData.Category.ToString());
        }
    }

#if WITH_EDITOR
    // Validate sound bases
    for (const FAudioCategorySoundData& CategoryData : CategoryDataArray)
    {
        if (CategoryData.Category == DemoSoundTags::Master)
        {
            continue;
        }
        checkf(CategoryData.Map, TEXT("No sound map for category: %s"), *CategoryData.Category.ToString());
        for (const auto& [SoundTag, SoundPtr] : *(CategoryData.Map))
        {
            if (SoundPtr.IsNull())
            {
                DemoLOG_CF(LogAudio, Error, TEXT("Null sound for tag: %s"), *SoundTag.ToString());
            }
        }
    }
#endif // WITH_EDITOR
}

void UDemoAudioSubsystem::LoadUserAudioSettings()
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        // Apply global SoundMix
        if (UWorld* World = GetWorld())
        {
            UGameplayStatics::PushSoundMixModifier(World, GlobalSoundMix);

            // Apply volume settings
            for (const FAudioCategorySoundData& CategoryData : CategoryDataArray)
            {
                if (CategoryData.SoundClass)
                {
                    const float* CategoryVolume = UserSettings->GetVolumeSetting(CategoryData.Category);
                    check(CategoryVolume);

                    UGameplayStatics::SetSoundMixClassOverride(World, GlobalSoundMix, CategoryData.SoundClass, *CategoryVolume, 1.f, 0.f);
                }
            }
        }
        else
        {
            DemoLOG_CF(LogAudio, Error, TEXT("Failed to get World. SoundMix and volume settings not applied."));
        }

        // Mute when unfocused
        bool bMuteWhenUnfocused = UserSettings->GetMuteWhenUnfocused();
        FApp::SetUnfocusedVolumeMultiplier(bMuteWhenUnfocused ? 0.f : 1.f);

        // Bind to changes
        if (!bHasBoundToUserSettings)
        {
            bHasBoundToUserSettings = true;
            UserSettings->OnAudioBoolSettingChanged.AddUObject(this, &ThisClass::HandleAudioBoolUserSettingChanged);
            UserSettings->OnAudioFloatSettingChanged.AddUObject(this, &ThisClass::HandleAudioFloatUserSettingChanged);
        }
    }
}

void UDemoAudioSubsystem::ClearAudioComponent()
{
    if (MusicAudioComponent)
    {
        MusicAudioComponent->Stop();
        MusicAudioComponent->DestroyComponent();
        MusicAudioComponent = nullptr;
    }
}

void UDemoAudioSubsystem::HandleWorldCleanup(UWorld* World, bool /*bSessionEnded*/, bool /*bCleanupResources*/)
{
    if (World)
    {
        UGameplayStatics::PopSoundMixModifier(World, GlobalSoundMix);
    }

    if (MusicAudioComponent && MusicAudioComponent->GetWorld() == World)
    {
        ClearAudioComponent();
    }
}

void UDemoAudioSubsystem::HandlePostLoadMapWithWorld(UWorld* LoadedWorld)
{
    if (LoadedWorld)
    {
        // Might not be necessary to push again, but to be safe.
        UGameplayStatics::PushSoundMixModifier(LoadedWorld, GlobalSoundMix);
    }
}

void UDemoAudioSubsystem::HandleAudioBoolUserSettingChanged(FGameplayTag InTag, bool bNewValue)
{
    // Mute when unfocused
    if (InTag == DemoGameplayTags::Setting_Audio_MuteWhenUnfocused)
    {
        FApp::SetUnfocusedVolumeMultiplier(bNewValue ? 0.f : 1.f);
    }
}

void UDemoAudioSubsystem::HandleAudioFloatUserSettingChanged(FGameplayTag InTag, float NewValue)
{
    // Update SoundClass volume
    if (UWorld* World = GetWorld())
    {
        for (const FAudioCategorySoundData& CategoryData : CategoryDataArray)
        {
            if (CategoryData.Category == InTag && CategoryData.SoundClass)
            {
                UGameplayStatics::SetSoundMixClassOverride(World, GlobalSoundMix, CategoryData.SoundClass, NewValue, 1.f, 0.f);
            }
        }
    }
}

FSoundQueryResult UDemoAudioSubsystem::GetSoundByTag(FGameplayTag SoundTag)
{
    const FAudioCategorySoundData* CategoryData = CategoryDataArray.FindByPredicate([SoundTag](const FAudioCategorySoundData& Data)
        {
            return SoundTag.MatchesTag(Data.Category);
        });
    if (!CategoryData)
    {
        DemoLOG_CF(LogAudio, Warning, TEXT("AudioSubsystem not initialized."));
        return {};
    }

    checkf(CategoryData->Map, TEXT("SoundCollection is not set in project settings, or garbage collected."));

    const TSoftObjectPtr<USoundBase>* SoundPtr = CategoryData->Map->Find(SoundTag);
    if (!SoundPtr)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("SoundCollection has no sound for tag: %s"), *SoundTag.ToString());
        return {};
    }

    USoundBase* Sound = SoundPtr->LoadSynchronous();
    if (!Sound)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Failed to load sound for tag: %s"), *SoundTag.ToString());
        return {};
    }

    return {Sound, CategoryData->Category};
}