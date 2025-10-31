// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StartUpPlayerController.generated.h"

class UMainMenuWidget;

/**
 *
 */
UCLASS()
class DEMO_API AStartUpPlayerController : public APlayerController
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    AStartUpPlayerController();

protected:
    virtual void BeginPlay() override;

public:
    void ShowMainMenu();

    void StartNewGame();

private:
    void InitMainMenu();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UMainMenuWidget> MainMenuWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    FName StartLevelName;
};