// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManagementSubsystem.generated.h"

/**
 * UI Management Subsystem.
 * Need to call Init() to initialize (not in constructor).
 * misc: Z order for AddToViewport
 */
UCLASS()
class DEMO_API UUIManagementSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI setup
    ////////////////////////////////////////////////////////
public:
    UUIManagementSubsystem() {}
};