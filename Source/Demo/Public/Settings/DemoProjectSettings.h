// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DemoTypes/ActionInfoEntryConfig.h"
#include "DemoTypes/ItemAnimLayerConfig.h"
#include "Widgets/Layout/SScaleBox.h"
#include "DemoProjectSettings.generated.h"

/**
 *
 */
UCLASS(Config = "Game", defaultconfig)
class DEMO_API UDemoProjectSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere, Category = "Action Info")
    TSoftObjectPtr<UActionInfoEntryConfig> ActionInfoEntryConfig;

    UPROPERTY(Config, EditAnywhere, Category = "Animation")
    TSoftObjectPtr<UItemAnimLayerConfig> ItemAnimLayerConfig;

    // Use images if MoviePath is empty.
    // Path == "Logo" means /Content/Movies/Logo.mp4
    UPROPERTY(Config, EditAnywhere, Category = "Loading Screen")
    FString MoviePath;

    UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "/Script/Engine.Texture2D"))
    TArray<FSoftObjectPath> LoadingScreenImages;

    UPROPERTY(Config, EditAnywhere, Category = "Loading Screen")
    TEnumAsByte<EStretch::Type> LoadingScreenImageStretch{EStretch::ScaleToFit};
};