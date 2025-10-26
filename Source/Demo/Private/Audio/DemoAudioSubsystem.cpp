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

    // Get DemoProjectSettings
    const UDemoProjectSettings* DemoProjectSettings = GetDefault<UDemoProjectSettings>();
    if (!DemoProjectSettings)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to get UDemoProjectSettings."));
        return;
    }

    // Load SoundCollection
    const USoundCollection* SoundCollection = DemoProjectSettings->SoundCollection.LoadSynchronous();
    if (!SoundCollection)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to load SoundCollection from DemoProjectSettings."));
        return;
    }

    // Initialize AudioMap
    // @TODO - Load volume settings from user settings.
    MasterVolume = 0.5f;
    AudioMap.Emplace(DemoSoundTags::Music, FAudioCategoryData{1.f, &SoundCollection->MusicMap});
    AudioMap.Emplace(DemoSoundTags::SFX, FAudioCategoryData{1.f, &SoundCollection->SFXMap});
    AudioMap.Emplace(DemoSoundTags::UI, FAudioCategoryData{1.f, &SoundCollection->UIMap});
    AudioMap.Emplace(DemoSoundTags::Voice, FAudioCategoryData{1.f, &SoundCollection->VoiceMap});

    // Validate AudioMap
    for (const auto& [CategoryTag, CategoryData] : AudioMap)
    {
        checkf(CategoryData.Map, TEXT("Audio category (%s) has no sound map."), *CategoryTag.ToString());
        DemoLOG_CF(LogDemoGame, Warning,
            TEXT("@TEST - Loaded audio category (%s) with %d sounds."), *CategoryTag.ToString(), CategoryData.Map->Num());
    }
}

void UDemoAudioSubsystem::PlaySound2D(const UObject* WorldContextObject, FGameplayTag SoundTag, float VolumeMultiplier) const
{
    auto [Sound, CategoryVolume] = GetSoundByTag(SoundTag);
    if (!Sound)
    {
        return;
    }

    const float FinalVolume = MasterVolume * CategoryVolume * VolumeMultiplier;
    UGameplayStatics::PlaySound2D(WorldContextObject, Sound, FinalVolume);
}

void UDemoAudioSubsystem::PlaySoundAtLocation(const UObject* WorldContextObject, FGameplayTag SoundTag, FVector Location, FRotator Rotation, float VolumeMultiplier) const
{
    auto [Sound, CategoryVolume] = GetSoundByTag(SoundTag);
    if (!Sound)
    {
        return;
    }

    const float FinalVolume = MasterVolume * CategoryVolume * VolumeMultiplier;
    UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Sound, Location, Rotation, FinalVolume);
}

void UDemoAudioSubsystem::PlayDefaultBGM(const UObject* WorldContextObject) const
{
    const FGameplayTag DefaultBGMTag = DemoSoundTags::Music_Exploration;
    PlaySound2D(WorldContextObject, DefaultBGMTag);
}

TPair<USoundBase*, float> UDemoAudioSubsystem::GetSoundByTag(FGameplayTag SoundTag) const
{
    const FAudioCategoryData* CategoryData = AudioMap.Find(DemoSoundTags::GetCategory(SoundTag));
    if (!CategoryData)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Sound tag (%s) is not valid."), *SoundTag.ToString());
        return {nullptr, 0.f};
    }

    const TSoftObjectPtr<USoundBase>* SoundPtr = CategoryData->Map->Find(SoundTag);
    if (!SoundPtr)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Sound tag (%s) not found from sound collection."), *SoundTag.ToString());
        return {nullptr, 0.f};
    }

    USoundBase* Sound = SoundPtr->LoadSynchronous();
    if (!Sound)
    {
        DemoLOG_CF(LogAudio, Error, TEXT("Failed to load sound for tag (%s)."), *SoundTag.ToString());
        return {nullptr, 0.f};
    }

    return {Sound, CategoryData->Volume};
}