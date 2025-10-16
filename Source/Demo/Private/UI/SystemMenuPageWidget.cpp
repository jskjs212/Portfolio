// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SystemMenuPageWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerController/DemoPlayerController.h"

void USystemMenuPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ContinueButton && SaveButton && LoadButton &&
        ExitToMainMenuButton && ExitToDesktopButton,
        TEXT("Failed to bind widgets."));

    SaveButton->SetIsEnabled(false);
    LoadButton->SetIsEnabled(false);

    ContinueButton->OnClicked.AddDynamic(this, &ThisClass::HandleContinueButtonClicked);
    ExitToMainMenuButton->OnClicked.AddDynamic(this, &ThisClass::HandleExitToMainMenuButtonClicked);
    ExitToDesktopButton->OnClicked.AddDynamic(this, &ThisClass::HandleExitToDesktopButtonClicked);
}

void USystemMenuPageWidget::HandleContinueButtonClicked()
{
    if (ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
    {
        DemoPlayerController->ShowPlayerMenu(false);
    }
}

void USystemMenuPageWidget::HandleExitToMainMenuButtonClicked()
{
    const FName MainMenuLevelName = TEXT("StartUp"); // @hardcoded
    UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}

void USystemMenuPageWidget::HandleExitToDesktopButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}