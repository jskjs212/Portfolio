// Fill out your copyright notice in the Description page of Project Settings.

#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Audio/SoundCollection.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/DemoProjectSettings.h"

void UDemoAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InitAudioMap();
}

void UDemoAudioSubsystem::PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag, float VolumeMultiplier)
{
    if (!WorldContextObject)
    {
        return;
    }

    auto [Sound, CategoryVolume] = GetSoundByTag(SoundTag);
    if (!Sound)
    {
        return;
    }

    const float FinalVolume = MasterVolume * CategoryVolume * VolumeMultiplier;
    UGameplayStatics::PlaySound2D(WorldContextObject, Sound, FinalVolume);
}

void UDemoAudioSubsystem::PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, FRotator Rotation, float VolumeMultiplier)
{
    if (!WorldContextObject)
    {
        return;
    }

    auto [Sound, CategoryVolume] = GetSoundByTag(SoundTag);
    if (!Sound)
    {
        return;
    }

    const float FinalVolume = MasterVolume * CategoryVolume * VolumeMultiplier;
    UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Sound, Location, Rotation, FinalVolume);
}

void UDemoAudioSubsystem::PlayDefaultBGM(const UObject* WorldContextObject)
{
    const FGameplayTag DefaultBGMTag = DemoSoundTags::Music_Exploration;
    PlaySound2D(WorldContextObject, DefaultBGMTag);
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
    MasterVolume = 0.5f;
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

TPair<USoundBase*, float> UDemoAudioSubsystem::GetSoundByTag(FGameplayTag SoundTag)
{
    const FGameplayTag Category = DemoSoundTags::GetCategory(SoundTag);
    if (!Category.IsValid())
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Invalid category for sound tag: %s"), *SoundTag.ToString());
        return {nullptr, 0.f};
    }

    const FAudioCategoryData* CategoryData = AudioMap.Find(Category);
    checkf(CategoryData, TEXT("No map for category: %s"), *Category.ToString());

    // @check - Access to the map pointer cause a crash if the SoundCollection has been GCed.
    const TSoftObjectPtr<USoundBase>* SoundPtr = CategoryData->Map->Find(SoundTag);
    if (!SoundPtr)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("SoundCollection has no sound for tag: %s"), *SoundTag.ToString());
        return {nullptr, 0.f};
    }

    USoundBase* Sound = SoundPtr->LoadSynchronous();
    if (!Sound)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Failed to load sound for tag: %s"), *SoundTag.ToString());
        return {nullptr, 0.f};
    }

    return {Sound, CategoryData->Volume};
}