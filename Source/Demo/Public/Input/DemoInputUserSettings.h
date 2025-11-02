// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "DemoInputUserSettings.generated.h"

class ULocalPlayer;

/**
 *
 */
UCLASS()
class DEMO_API UDemoInputUserSettings : public UEnhancedInputUserSettings
{
    GENERATED_BODY()

public:
    // Get the EnhancedInputUserSettings for the given local player.
    static UEnhancedInputUserSettings* GetUserSettingsForLocalPlayer(const ULocalPlayer* LocalPlayer);
};