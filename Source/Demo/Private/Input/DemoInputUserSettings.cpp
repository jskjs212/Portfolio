// Fill out your copyright notice in the Description page of Project Settings.

#include "Input/DemoInputUserSettings.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

UEnhancedInputUserSettings* UDemoInputUserSettings::GetUserSettingsForLocalPlayer(const ULocalPlayer* LocalPlayer)
{
    if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
    {
        return EISubsystem->GetUserSettings();
    }
    return nullptr;
}