// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AnimationDataSubsystem.h"
#include "DemoTypes/ActionInfoConfig.h"
#include "DemoTypes/ActionInfoEntryConfig.h"
#include "DemoTypes/LogCategories.h"
#include "Settings/DemoProjectSettings.h"

void UAnimationDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Get DemoProjectSettings
    const UDemoProjectSettings* DemoProjectSettings = GetDefault<UDemoProjectSettings>();
    if (!DemoProjectSettings)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to get UDemoProjectSettings."));
        return;
    }

    // Load ActionInfoEntryConfig
    const UActionInfoEntryConfig* ActionInfoEntryConfig = DemoProjectSettings->ActionInfoEntryConfig.LoadSynchronous();
    if (!ActionInfoEntryConfig)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to load ActionInfoEntryConfig from DemoProjectSettings."));
        return;
    }

    // Load ItemAnimLayerConfig
    const UItemAnimLayerConfig* ItemAnimLayerConfig = DemoProjectSettings->ItemAnimLayerConfig.LoadSynchronous();
    if (!ItemAnimLayerConfig)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to load ItemAnimLayerConfig from DemoProjectSettings."));
        return;
    }

    // Populate the map from the array
    // DemoProjectSettings -> ActionInfoConfig -> ActionInfoEntries -> ActionInfoMap
    for (const FActionInfoEntry& Entry : ActionInfoEntryConfig->ActionInfoEntries)
    {
        // @check - Async? Optimize?
        const UActionInfoConfig* LoadedActionInfo = Entry.ActionInfo.LoadSynchronous();
        if (LoadedActionInfo)
        {
            if (ActionInfoMap.Contains(Entry.Key))
            {
                DemoLOG_CF(LogDemoGame, Error, TEXT("Duplicate ActionInfoKey (%s, %s). Skipping."), *Entry.Key.CharacterTag.ToString(), *Entry.Key.WeaponTag.ToString());
                continue;
            }

            ActionInfoMap.Add(Entry.Key, LoadedActionInfo);
        }
    }

    ItemAnimLayerMap = ItemAnimLayerConfig->ItemAnimLayerMap;

#if WITH_EDITOR
    // Validate all ActionInfoConfigs
    for (const auto& [ActionInfoKey, ActionInfoConfig] : ActionInfoMap)
    {
        if (!ActionInfoKey.CharacterTag.IsValid() || !ActionInfoKey.WeaponTag.IsValid() || !ActionInfoConfig->IsValid())
        {
            DemoLOG_CF(LogDemoGame, Error, TEXT("Invalid ActionInfoConfig (%s, %s)."), *ActionInfoKey.CharacterTag.ToString(), *ActionInfoKey.WeaponTag.ToString());
        }
    }
#endif // WITH_EDITOR
}

void UAnimationDataSubsystem::Deinitialize()
{
    ActionInfoMap.Empty();
    Super::Deinitialize();
}