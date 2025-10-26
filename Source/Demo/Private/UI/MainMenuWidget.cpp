// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuWidget.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Components/Button.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIsFocusable(true);

    checkf(NewGameButton && LoadButton && QuitButton, TEXT("Failed to bind widgets."));

    if (StartLevelName == NAME_None)
    {
        DemoLOG_CF(LogUI, Error, TEXT("StartLevelName is not set."));
        StartLevelName = TEXT("Test");
    }

    LoadButton->SetIsEnabled(false);

    NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleNewGameButtonClicked);
    QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleQuitButtonClicked);

    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(this, DemoSoundTags::Music_MainMenu);
    }
}

void UMainMenuWidget::HandleNewGameButtonClicked()
{
    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        FInputModeGameOnly InputModeData;
        PlayerController->SetInputMode(InputModeData);
        PlayerController->SetShowMouseCursor(false);
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("PlayerController is not valid."));
    }

    UGameplayStatics::OpenLevel(this, StartLevelName);
}

void UMainMenuWidget::HandleQuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}