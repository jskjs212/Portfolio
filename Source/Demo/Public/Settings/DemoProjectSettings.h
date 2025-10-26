// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Widgets/Layout/SScaleBox.h"
#include "DemoProjectSettings.generated.h"

class UActionInfoEntryConfig;
class UItemAnimLayerConfig;
class USoundCollection;

/**
 *
 */
UCLASS(Config = "Game", defaultconfig)
class DEMO_API UDemoProjectSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, EditDefaultsOnly, Category = "Action Info")
    TSoftObjectPtr<UActionInfoEntryConfig> ActionInfoEntryConfig;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Animation")
    TSoftObjectPtr<UItemAnimLayerConfig> ItemAnimLayerConfig;

    // Use images if MoviePath is empty.
    // Path == "Logo" means /Content/Movies/Logo.mp4
    UPROPERTY(Config, EditDefaultsOnly, Category = "Loading Screen")
    FString MoviePath;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Loading Screen", meta = (AllowedClasses = "/Script/Engine.Texture2D"))
    TArray<FSoftObjectPath> LoadingScreenImages;

    UPROPERTY(Config, EditDefaultsOnly, Category = "Loading Screen")
    TEnumAsByte<EStretch::Type> LoadingScreenImageStretch{EStretch::ScaleToFit};

    UPROPERTY(Config, EditDefaultsOnly, Category = "Audio")
    TSoftObjectPtr<USoundCollection> SoundCollection;
};