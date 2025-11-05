// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SystemMenuPageWidget.h"
#include "Character/PlayerCharacter.h"
#include "Components/Button.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/DemoPlayerController.h"
#include "Settings/DemoSaveGame.h"
#include "System/DemoGameInstance.h"

USystemMenuPageWidget::USystemMenuPageWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
    AsyncSavedDelegate{FAsyncSaveGameToSlotDelegate::CreateUObject(
        this, &ThisClass::HandleAsyncSaveCompleted)}
{
}

void USystemMenuPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(ContinueButton && SaveButton && LoadButton &&
        ExitToMainMenuButton && ExitToDesktopButton,
        TEXT("Failed to bind widgets."));

    bool bSaveExists = UGameplayStatics::DoesSaveGameExist(
        UDemoSaveGame::DefaultSaveSlot, UDemoSaveGame::DefaultUserIndex);
    LoadButton->SetIsEnabled(bSaveExists);

    ContinueButton->OnClicked.AddDynamic(this, &ThisClass::HandleContinueButtonClicked);
    SaveButton->OnClicked.AddDynamic(this, &ThisClass::HandleSaveButtonClicked);
    LoadButton->OnClicked.AddDynamic(this, &ThisClass::HandleLoadButtonClicked);
    ExitToMainMenuButton->OnClicked.AddDynamic(this, &ThisClass::HandleExitToMainMenuButtonClicked);
    ExitToDesktopButton->OnClicked.AddDynamic(this, &ThisClass::HandleExitToDesktopButtonClicked);

    if (UDemoGameInstance* DemoGameInstance = GetGameInstance<UDemoGameInstance>())
    {
        DemoGameInstance->OnLoadedSaveGameDiscarded.AddUObject(this, &ThisClass::HandleLoadedSaveGameDiscarded);
    }
}

void USystemMenuPageWidget::NativeDestruct()
{
    if (UDemoGameInstance* DemoGameInstance = GetGameInstance<UDemoGameInstance>())
    {
        DemoGameInstance->OnLoadedSaveGameDiscarded.RemoveAll(this);
    }
    Super::NativeDestruct();
}

void USystemMenuPageWidget::HandleContinueButtonClicked()
{
    if (ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
    {
        DemoPlayerController->ShowPlayerMenu(false);
    }
}

void USystemMenuPageWidget::HandleSaveButtonClicked()
{
    if (UDemoGameInstance* DemoGameInstance = GetGameInstance<UDemoGameInstance>())
    {
        SaveButton->SetIsEnabled(false);
        DemoGameInstance->RequestSaveGame(
            UDemoSaveGame::DefaultSaveSlot,
            UDemoSaveGame::DefaultUserIndex,
            AsyncSavedDelegate);
    }
}

void USystemMenuPageWidget::HandleAsyncSaveCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
    UE_LOG(LogDemoGame, Display, TEXT("Save completed. Slot: %s, UserIndex: %d, Success: %s"),
        *SlotName, UserIndex, bSuccess ? TEXT("true") : TEXT("false"));

    SaveButton->SetIsEnabled(true);
    if (bSuccess)
    {
        LoadButton->SetIsEnabled(true);
    }
}

void USystemMenuPageWidget::HandleLoadButtonClicked()
{
    bool bSaveExists = UGameplayStatics::DoesSaveGameExist(
        UDemoSaveGame::DefaultSaveSlot, UDemoSaveGame::DefaultUserIndex);
    if (!bSaveExists)
    {
        UE_LOG(LogDemoGame, Warning, TEXT("No save exists."));
        return;
    }

    UDemoSaveGame* LoadedSaveGame = Cast<UDemoSaveGame>(
        UGameplayStatics::LoadGameFromSlot(UDemoSaveGame::DefaultSaveSlot, UDemoSaveGame::DefaultUserIndex));
    if (!LoadedSaveGame)
    {
        UE_LOG(LogDemoGame, Display, TEXT("Failed to load game from slot: %s, UserIndex: %d"),
            *UDemoSaveGame::DefaultSaveSlot, UDemoSaveGame::DefaultUserIndex);
        return;
    }

    if (UDemoGameInstance* DemoGameInstance = GetGameInstance<UDemoGameInstance>())
    {
        DemoGameInstance->RequestLoadGame(LoadedSaveGame);
        DemoGameInstance->ApplyLoadedSaveGame();
    }
}

void USystemMenuPageWidget::HandleLoadedSaveGameDiscarded()
{
    LoadButton->SetIsEnabled(false);
}

void USystemMenuPageWidget::HandleExitToMainMenuButtonClicked()
{
    if (ADemoPlayerController* DemoPlayerController = GetOwningPlayer<ADemoPlayerController>())
    {
        DemoPlayerController->GoToMainMenu();
    }
}

void USystemMenuPageWidget::HandleExitToDesktopButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}