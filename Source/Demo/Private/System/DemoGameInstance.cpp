// Fill out your copyright notice in the Description page of Project Settings.

#include "System/DemoGameInstance.h"
#include "MoviePlayer.h"
#include "Settings/DemoProjectSettings.h"
#include "UI/SBackgroundWidget.h"
#include "Widgets/SOverlay.h"

void UDemoGameInstance::Init()
{
    Super::Init();

    FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::BeginLoadingScreen);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::EndLoadingScreen);
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