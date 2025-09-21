// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DemoTypes/ActionInfoEntryConfig.h"
#include "DemoTypes/ItemAnimLayerConfig.h"
#include "DemoProjectSettings.generated.h"

/**
 *
 */
UCLASS(Config = Game)
class DEMO_API UDemoProjectSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    // Key {CharacterTag
    UPROPERTY(EditAnywhere, Config, Category = "Action Info")
    TSoftObjectPtr<UActionInfoEntryConfig> ActionInfoEntryConfig;

    UPROPERTY(EditAnywhere, Config, Category = "Animation")
    TSoftObjectPtr<UItemAnimLayerConfig> ItemAnimLayerConfig;
};