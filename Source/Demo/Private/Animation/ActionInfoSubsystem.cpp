// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/ActionInfoSubsystem.h"
#include "DemoTypes/ActionInfoConfig.h"

void UActionInfoSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Populate the map from the array
    for (const FActionInfoEntry& Entry : ActionInfoEntries)
    {
        // @TODO - Async?
        UActionInfoConfig* LoadedActionInfo = Entry.ActionInfo.LoadSynchronous();
        if (LoadedActionInfo)
        {
            ActionInfoMap.Add(Entry.Key, LoadedActionInfo);
        }
    }

#if WITH_EDITOR
    // Validate all ActionInfoConfigs
    for (const auto& [ActionInfoKey, ActionInfoConfig] : ActionInfoMap)
    {
        if (!ActionInfoKey.IdentityTag.IsValid() || !ActionInfoKey.WeaponTag.IsValid() || !ActionInfoConfig->IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("UActionInfoSubsystem::Initialize: Invalid ActionInfoConfig (%s, %s)."), *ActionInfoKey.IdentityTag.ToString(), *ActionInfoKey.WeaponTag.ToString());
        }
    }
#endif // WITH_EDITOR

    // @TEST
    UE_LOG(LogTemp, Warning, TEXT("UActionInfoSubsystem::Initialize - %s"), *GetName());
}

void UActionInfoSubsystem::Deinitialize()
{
    ActionInfoMap.Empty();
    Super::Deinitialize();
}

const UActionInfoConfig* UActionInfoSubsystem::GetActionInfoConfig(FGameplayTag IdentityTag, FGameplayTag WeaponTag) const
{
    const FActionInfoKey Key{IdentityTag, WeaponTag};
    if (const TObjectPtr<UActionInfoConfig>* ActionInfoPtr = ActionInfoMap.Find(Key))
    {
        return *ActionInfoPtr;
    }

    // Invalid key
    UE_LOG(LogTemp, Error, TEXT("UActionInfoSubsystem::GetActionInfoConfig: No ActionInfoConfig found for (%s, %s)."), *IdentityTag.ToString(), *WeaponTag.ToString());

    return nullptr;
}