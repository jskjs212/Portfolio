// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/StartUpPlayerController.h"
#include "StartUpGameMode.generated.h"

UCLASS()
class DEMO_API AStartUpGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AStartUpGameMode()
    {
        PlayerControllerClass = AStartUpPlayerController::StaticClass();
    }
};