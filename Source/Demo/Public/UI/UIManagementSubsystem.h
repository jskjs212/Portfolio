// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManagementSubsystem.generated.h"

class UMainMenuWidget;

/**
 * UI Management Subsystem.
 */
UCLASS()
class DEMO_API UUIManagementSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI setup
    ////////////////////////////////////////////////////////
public:
    UUIManagementSubsystem();

    void ShowMainMenu(APlayerController* InPlayerController);

private:
    void InitMainMenu();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UMainMenuWidget> MainMenuWidget;
};