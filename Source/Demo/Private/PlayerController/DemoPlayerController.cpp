// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/DemoPlayerController.h"
#include "Character/PlayerCharacter.h"
#include "Items/Item.h"
#include "UI/DemoHUD.h"
#include "UI/DemoHUDWidget.h"
#include "UI/InventoryPageWidget.h"
#include "UI/PlayerMenuWidget.h"

ADemoPlayerController::ADemoPlayerController()
{
    // hardcoded:
    static ConstructorHelpers::FClassFinder<UPlayerMenuWidget> PlayerMenuWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_PlayerMenu.WBP_PlayerMenu_C'"));
    if (PlayerMenuWidgetBPClass.Succeeded())
    {
        PlayerMenuWidgetClass = PlayerMenuWidgetBPClass.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ADemoHUD - PlayerMenuWidget BP not found."));
        PlayerMenuWidgetClass = UPlayerMenuWidget::StaticClass();
    }
}

void ADemoPlayerController::ShowPlayerMenu(bool bShow)
{
    if (bShow)
    {
        // Input mode: UI only
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(PlayerMenuWidget->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputModeData);

        // Set mouse position to center
        int32 ViewportX;
        int32 ViewportY;
        GetViewportSize(ViewportX, ViewportY);
        SetMouseLocation(ViewportX / 2, ViewportY / 2);

        SetIgnoreLookInput(true);
        SetIgnoreMoveInput(true);
        SetShowMouseCursor(true);

        PlayerMenuWidget->SetVisibilityAndFocus(true);

        // TODO: IsPendingUIUpdate? Update

        if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
        {
            if (DemoHUD->DemoHUDWidget)
            {
                DemoHUD->DemoHUDWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
    else // hide
    {
        FInputModeGameOnly InputModeData;
        SetInputMode(InputModeData);

        SetIgnoreLookInput(false);
        SetIgnoreMoveInput(false);
        SetShowMouseCursor(false);

        PlayerMenuWidget->SetVisibilityAndFocus(false);

        // TODO: Cancel drag

        if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
        {
            if (DemoHUD->DemoHUDWidget)
            {
                DemoHUD->DemoHUDWidget->SetVisibility(ESlateVisibility::Visible);
            }
        }
    }
}

void ADemoPlayerController::ShowInventoryContextMenu(const FItemSlot& InSlot, int32 DesignatedIndex)
{
    PlayerMenuWidget->InventoryPageWidget->ShowContextMenu(InSlot, DesignatedIndex);
}

void ADemoPlayerController::InitDemoHUD()
{
    ADemoHUD* DemoHUD = GetHUD<ADemoHUD>();
    if (DemoHUD)
    {
        DemoHUD->Init();

        // Bind functions that update HUD widgets to character delegates.
        if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
        {
            PlayerCharacter->OnInteractableFocused.BindUObject(this, &ADemoPlayerController::HandleInteractableFocused);
        }
    }
}

void ADemoPlayerController::InitPlayerMenu()
{
    if (!PlayerMenuWidgetClass)
    {
        return;
    }

    PlayerMenuWidget = CreateWidget<UPlayerMenuWidget>(this, PlayerMenuWidgetClass);
    if (PlayerMenuWidget)
    {
        PlayerMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        PlayerMenuWidget->AddToViewport(1);
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