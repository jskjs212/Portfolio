// Fill out your copyright notice in the Description page of Project Settings.

#include "System/DemoGameInstance.h"
#include "Character/PlayerCharacter.h"
#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "DemoTypes/LogCategories.h"
#include "MoviePlayer.h"
#include "Settings/DemoProjectSettings.h"
#include "Settings/DemoSaveGame.h"
#include "UI/SBackgroundWidget.h"

void UDemoGameInstance::Init()
{
    Super::Init();

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::BeginLoadingScreen);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::EndLoadingScreen);
}

void UDemoGameInstance::RequestSaveGame(const FString& InSaveSlot, int32 InUserIndex, FAsyncSaveGameToSlotDelegate InSavedDelegate)
{
    // No save logic elsewhere. So this UI handles the save logic itself.
    UDemoSaveGame* NewSaveGame = Cast<UDemoSaveGame>(UGameplayStatics::CreateSaveGameObject(UDemoSaveGame::StaticClass()));
    if (!ensure(NewSaveGame))
    {
        return;
    }

    // Version
    NewSaveGame->SavedSaveGameVersion = UDemoSaveGame::CurrentSaveGameVersion;

    // Save the game data.
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (ensure(PlayerController))
    {
        APlayerCharacter* PlayerCharacter = PlayerController->GetPawn<APlayerCharacter>();
        if (ensure(PlayerCharacter))
        {
            // Player character
            PlayerCharacter->PopulateSaveData(NewSaveGame->PlayerCharacterData);

            // Equipment
            UEquipmentComponent* EquipmentComponent = PlayerCharacter->FindComponentByClass<UEquipmentComponent>();
            if (ensure(EquipmentComponent))
            {
                EquipmentComponent->PopulateSaveData(NewSaveGame->EquipmentData);
            }
        }

        // Inventory
        UInventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UInventoryComponent>();
        if (ensure(InventoryComponent))
        {
            InventoryComponent->PopulateSaveData(NewSaveGame->InventoryData);
        }
    }

    DemoLOG_CF(LogDemoGame, Display, TEXT("Saving game."));

    UGameplayStatics::AsyncSaveGameToSlot(NewSaveGame, UDemoSaveGame::DefaultSaveSlot, UDemoSaveGame::DefaultUserIndex, InSavedDelegate);
}

void UDemoGameInstance::RequestLoadGame(const UDemoSaveGame* InLoadedSaveGame)
{
    bPendingLoadGame = true;
    LoadedSaveGame = InLoadedSaveGame;
}

bool UDemoGameInstance::LoadedSaveGameVersionCheck(const UDemoSaveGame* InLoadedSaveGame) const
{
    if (InLoadedSaveGame)
    {
        if (InLoadedSaveGame->SavedSaveGameVersion >= UDemoSaveGame::CurrentSaveGameVersion)
        {
            return true;
        }
    }

    // @TODO - Move file to other place (backup)
    UGameplayStatics::DeleteGameInSlot(UDemoSaveGame::DefaultSaveSlot, UDemoSaveGame::DefaultUserIndex);

    // @TODO - Handle old versions.
    DemoLOG_CF(LogDemoGame, Error, TEXT("Loaded save game version (%.2f) is older than current version (%.2f). Aborting load."),
        InLoadedSaveGame->SavedSaveGameVersion, UDemoSaveGame::CurrentSaveGameVersion);
    OnLoadedSaveGameDiscarded.Broadcast();
    return false;
}

void UDemoGameInstance::ApplyLoadedSaveGame()
{
    if (!bPendingLoadGame || !LoadedSaveGame)
    {
        return;
    }

    if (!LoadedSaveGameVersionCheck(LoadedSaveGame))
    {
        return;
    }

    // @check - need more validation?

    // Apply the loaded SaveGame data.
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (ensure(PlayerController))
    {
        APlayerCharacter* PlayerCharacter = PlayerController->GetPawn<APlayerCharacter>();
        if (ensure(PlayerCharacter))
        {
            // Player character
            PlayerCharacter->LoadFromSaveData(LoadedSaveGame->PlayerCharacterData);

            // Equipment
            UEquipmentComponent* EquipmentComponent = PlayerCharacter->FindComponentByClass<UEquipmentComponent>();
            if (ensure(EquipmentComponent))
            {
                EquipmentComponent->LoadFromSaveData(LoadedSaveGame->EquipmentData);
            }
        }

        // Inventory
        UInventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UInventoryComponent>();
        if (ensure(InventoryComponent))
        {
            InventoryComponent->LoadFromSaveData(LoadedSaveGame->InventoryData);
        }
    }

    DemoLOG_CF(LogDemoGame, Display, TEXT("Loaded game."));

    // Reset the flags.
    bPendingLoadGame = false;
    LoadedSaveGame = nullptr;
}

void UDemoGameInstance::BeginLoadingScreen(const FString& InMapName)
{
    if (!IsRunningDedicatedServer())
    {
        FLoadingScreenAttributes LoadingScreen;

        const FString& MoviePath = GetDefault<UDemoProjectSettings>()->MoviePath;
        if (!MoviePath.IsEmpty())
        {
            LoadingScreen.MoviePaths.Add(MoviePath);
            LoadingScreen.bAllowInEarlyStartup = true;
            LoadingScreen.PlaybackType = EMoviePlaybackType::MT_LoadingLoop;
        }
        else // No movie, use images
        {
            LoadingScreen.WidgetLoadingScreen = SNew(SBackgroundWidget);
        }

        GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
    }
}

void UDemoGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
    // Do nothing for now.
}