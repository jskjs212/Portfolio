// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "DemoGameInstance.generated.h"

class UDemoSaveGame;

/**
 * Game instance class for Demo project.
 * Manages saving, loading, and loading screen.
 * Save/load: Only for first local player for now.
 */
UCLASS()
class DEMO_API UDemoGameInstance : public UGameInstance
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FSimpleMulticastDelegate OnLoadedSaveGameDiscarded;

    ////////////////////////////////////////////////////////
    //        Game instance functions
    ////////////////////////////////////////////////////////
public:
    virtual void Init() override;

    // Populate SaveGame object and request AsyncSaveGameToSlot.
    void RequestSaveGame(const FString& InSaveSlot, int32 InUserIndex, FAsyncSaveGameToSlotDelegate InSavedDelegate = FAsyncSaveGameToSlotDelegate{});

    // Just store the loaded SaveGame for applying later.
    void RequestLoadGame(const UDemoSaveGame* InLoadedSaveGame);

    // @return true if no problems found.
    bool LoadedSaveGameVersionCheck(const UDemoSaveGame* InLoadedSaveGame) const;

    // Apply the loaded SaveGame.
    void ApplyLoadedSaveGame();

    UFUNCTION()
    void BeginLoadingScreen(const FString& InMapName);

    UFUNCTION()
    void EndLoadingScreen(UWorld* InLoadedWorld);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    bool bPendingLoadGame{false};

    UPROPERTY()
    TObjectPtr<const UDemoSaveGame> LoadedSaveGame;
};