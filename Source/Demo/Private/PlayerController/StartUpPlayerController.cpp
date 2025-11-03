// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/StartUpPlayerController.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/UITypes.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenuWidget.h"

AStartUpPlayerController::AStartUpPlayerController()
{
    // @hardcoded
    static ConstructorHelpers::FClassFinder<UMainMenuWidget> MainMenuWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_MainMenu.WBP_MainMenu_C'"));
    if (MainMenuWidgetBPClass.Succeeded())
    {
        MainMenuWidgetClass = MainMenuWidgetBPClass.Class;
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("MainMenuWidget BP is not found."));
        MainMenuWidgetClass = UMainMenuWidget::StaticClass();
    }
}

void AStartUpPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (StartLevelName == NAME_None)
    {
        // @hardcoded
        StartLevelName = TEXT("Test");
    }

    ShowMainMenu();

#if WITH_EDITOR
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        // Lazy load for editor
        AudioSubsystem->LoadUserAudioSettings();
    }
#endif // WITH_EDITOR
}

void AStartUpPlayerController::ShowMainMenu()
{
    if (!MainMenuWidget)
    {
        InitMainMenu();
        if (!MainMenuWidget)
        {
            DemoLOG_CF(LogUI, Error, TEXT("Failed to create MainMenuWidget."));
            return;
        }
    }

    FInputModeUIOnly InputModeData;
    InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
    SetInputMode(InputModeData);
    SetShowMouseCursor(true);

    MainMenuWidget->AddToViewport(Demo::ZOrder::HUD_Base);
}

void AStartUpPlayerController::StartNewGame()
{
    FInputModeGameOnly InputModeData;
    SetInputMode(InputModeData);
    SetShowMouseCursor(false);

    UGameplayStatics::OpenLevel(this, StartLevelName);
}

void AStartUpPlayerController::InitMainMenu()
{
    if (!MainMenuWidgetClass)
    {
        DemoLOG_CF(LogUI, Error, TEXT("MainMenuWidgetClass is not set."));
        return;
    }

    MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);

    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlayMusic(this, DemoSoundTags::Music_MainMenu);
    }
}