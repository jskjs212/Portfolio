// Fill out your copyright notice in the Description page of Project Settings.

#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Audio/SoundCollection.h"
#include "Components/AudioComponent.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/DemoProjectSettings.h"

UDemoAudioSubsystem::UDemoAudioSubsystem()
{
    DefaultMusicTag = DemoSoundTags::Music_Exploration;
    MusicAudioComponent = nullptr;
}

void UDemoAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitAudioMap();

    FDelegateHandle Handle = FWorldDelegates::OnWorldCleanup.AddUObject(this, &ThisClass::HandleWorldCleanup);
}

void UDemoAudioSubsystem::Deinitialize()
{
    Super::Deinitialize();

    if (WorldCleanupHandle.IsValid())
    {
        FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupHandle);
        WorldCleanupHandle.Reset();
    }

    ClearAudioComponent();
}

void UDemoAudioSubsystem::PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag)
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

    const bool bIsUISound = Result.Category == DemoSoundTags::UI;
    const float FinalVolume = MasterVolume * Result.CategoryVolume;
    UGameplayStatics::PlaySound2D(
        WorldContextObject,
        Result.Sound,
        FinalVolume,
        1.f,     /* PitchMultiplier */
        0.f,     /* StartTime */
        nullptr, /* ConcurrencySettings */
        nullptr, /* OwningActor */
        bIsUISound
    );
}

void UDemoAudioSubsystem::PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, FRotator Rotation)
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

    const float FinalVolume = MasterVolume * Result.CategoryVolume;
    UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Result.Sound, Location, Rotation, FinalVolume);
}

void UDemoAudioSubsystem::PlayMusic(const UObject* WorldContextObject, FGameplayTag MusicTag)
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
    const float FinalVolume = MasterVolume * Result.CategoryVolume;
    MusicAudioComponent->SetSound(Result.Sound);
    MusicAudioComponent->SetVolumeMultiplier(FinalVolume);
    MusicAudioComponent->Play(0.f);
}

void UDemoAudioSubsystem::InitAudioMap()
{
    // Get DemoProjectSettings
    const UDemoProjectSettings* DemoProjectSettings = GetDefault<UDemoProjectSettings>();
    if (!DemoProjectSettings)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to get UDemoProjectSettings."));
        return;
    }

    // Load SoundCollection
    const USoundCollection* LoadedSoundCollection = DemoProjectSettings->SoundCollection.LoadSynchronous();
    if (!LoadedSoundCollection)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to load SoundCollection from DemoProjectSettings."));
        return;
    }
    SoundCollection = LoadedSoundCollection;

    // Initialize AudioMap
    // @TODO - Load volume settings from user settings.
    MasterVolume = 0.8f;
    AudioMap.Emplace(DemoSoundTags::Music, FAudioCategoryData{1.f, &SoundCollection->MusicMap});
    AudioMap.Emplace(DemoSoundTags::SFX, FAudioCategoryData{1.f, &SoundCollection->SFXMap});
    AudioMap.Emplace(DemoSoundTags::UI, FAudioCategoryData{1.f, &SoundCollection->UIMap});
    AudioMap.Emplace(DemoSoundTags::Voice, FAudioCategoryData{1.f, &SoundCollection->VoiceMap});

    // @TEST - Validate AudioMap
    for (const auto& [CategoryTag, CategoryData] : AudioMap)
    {
        checkf(CategoryData.Map, TEXT("No sound map for category: %s"), *CategoryTag.ToString());
        for (const auto& [SoundTag, SoundPtr] : *(CategoryData.Map))
        {
            checkf(!SoundPtr.IsNull(), TEXT("Null sound for tag: %s"), *SoundTag.ToString());
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
    if (MusicAudioComponent && MusicAudioComponent->GetWorld() == World)
    {
        ClearAudioComponent();
    }
}

FSoundQueryResult UDemoAudioSubsystem::GetSoundByTag(FGameplayTag SoundTag)
{
    const FGameplayTag Category = DemoSoundTags::GetCategory(SoundTag);
    if (!Category.IsValid())
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Invalid category for sound tag: %s"), *SoundTag.ToString());
        return {};
    }

    const FAudioCategoryData* CategoryData = AudioMap.Find(Category);
    checkf(CategoryData, TEXT("No map for category: %s"), *Category.ToString());

    // @check - Access to the map pointer cause a crash if the SoundCollection has been GCed.
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

    return {Sound, Category, CategoryData->Volume};
}