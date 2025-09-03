// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManagementSubsystem.generated.h"

class AItem;
class UInteractWidget;

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
    UUIManagementSubsystem();

    void Init();

    void InitInteractWidget();

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    // Show or hide item info widget.
    void ShowInteractWidget(const AItem* Item);

    ////////////////////////////////////////////////////////
    //        Variables - Widgets
    ////////////////////////////////////////////////////////
private:
    TSubclassOf<UInteractWidget> InteractWidgetClass;

    TObjectPtr<UInteractWidget> InteractWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    const AItem* CachedShowInfoItem{nullptr};
};