// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/DemoPlayerController.h"
#include "Character/PlayerCharacter.h"
#include "Items/Item.h"
#include "UI/DemoHUD.h"
#include "UI/DemoHUDWidget.h"

void ADemoPlayerController::InitDemoHUD()
{
    // Init HUD
    ADemoHUD* DemoHUD = GetHUD<ADemoHUD>();
    if (DemoHUD)
    {
        DemoHUD->Init();
    }

    // Bind functions that update HUD widgets to character delegates.
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
    {
        PlayerCharacter->OnInteractableFocused.BindUObject(this, &ADemoPlayerController::HandleInteractableFocused);
    }
}

void ADemoPlayerController::HandleInteractableFocused(IInteractable* NewFocusedInteractable)
{
    if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
    {
        if (DemoHUD->DemoHUDWidget)
        {
            DemoHUD->DemoHUDWidget->UpdateInteractWidgets(NewFocusedInteractable);
        }
    }
}