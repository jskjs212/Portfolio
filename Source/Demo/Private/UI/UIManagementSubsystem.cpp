// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UIManagementSubsystem.h"
#include "DemoTypes/LogCategories.h"
#include "UI/MainMenuWidget.h"

UUIManagementSubsystem::UUIManagementSubsystem()
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

void UUIManagementSubsystem::ShowMainMenu(APlayerController* InPlayerController)
{
    if (!InPlayerController)
    {
        InPlayerController = GetGameInstance()->GetFirstLocalPlayerController();
        if (!InPlayerController)
        {
            DemoLOG_CF(LogUI, Error, TEXT("PlayerController is not valid."));
            return;
        }
    }

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
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InPlayerController->SetInputMode(InputModeData);
    InPlayerController->SetShowMouseCursor(true);

    MainMenuWidget->AddToViewport();
}

void UUIManagementSubsystem::InitMainMenu()
{
    if (!MainMenuWidgetClass)
    {
        DemoLOG_CF(LogUI, Error, TEXT("MainMenuWidgetClass is not set."));
        return;
    }

    MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
}