// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DemoTypes/ActionInfoConfig.h"
#include "DemoProjectSettings.generated.h"

/**
 *
 */
UCLASS(Config = Game)
class DEMO_API UDemoProjectSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    // Each ActionInfoConfig should be associated with a unique {IdentityTag, WeaponTag} pair.
    UPROPERTY(EditAnywhere, Config, Category = "Action Info")
    TArray<FActionInfoEntry> DefaultActionInfoEntries;
};