// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DemoTypes/ActionInfoConfig.h"
#include "ActionInfoEntryConfig.generated.h"

/**
 * Helper struct to make it easier to edit TMap's entries in the editor.
 */
USTRUCT()
struct FActionInfoEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FCharacterWeaponKey Key;

    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<UActionInfoConfig> ActionInfo;
};

/**
 *
 */
UCLASS()
class DEMO_API UActionInfoEntryConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // Each ActionInfoConfig should be associated with a unique {CharacterTag, WeaponTag} pair.
    UPROPERTY(EditAnywhere, Category = "Action Info")
    TArray<FActionInfoEntry> ActionInfoEntries;
};