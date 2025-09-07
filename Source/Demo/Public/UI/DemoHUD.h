// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DemoHUD.generated.h"

class UDemoHUDWidget;

/**
 * Need to call Init().
 */
UCLASS()
class DEMO_API ADemoHUD : public AHUD
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    ADemoHUD();

    void Init();

    //void SetHUDVisibility(bool bVisible);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    TSubclassOf<UDemoHUDWidget> DemoHUDWidgetClass;

    TObjectPtr<UDemoHUDWidget> DemoHUDWidget;
};