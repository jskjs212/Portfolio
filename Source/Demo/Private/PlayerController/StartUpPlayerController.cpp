// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/StartUpPlayerController.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "UI/UIManagementSubsystem.h"

void AStartUpPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UUIManagementSubsystem* UIManagementSubsystem = UGameInstance::GetSubsystem<UUIManagementSubsystem>(GetGameInstance()))
    {
        UIManagementSubsystem->ShowMainMenu(this);
    }

    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(this, DemoSoundTags::Music_MainMenu);
    }
}