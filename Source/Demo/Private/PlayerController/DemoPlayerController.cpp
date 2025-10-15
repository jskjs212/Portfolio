// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/DemoPlayerController.h"
#include "Character/PlayerCharacter.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/GameplayStatics.h"
#include "UI/DemoHUD.h"
#include "UI/DemoHUDWidget.h"
#include "UI/PlayerMenuWidget.h"

ADemoPlayerController::ADemoPlayerController()
{
    // @hardcoded
    static ConstructorHelpers::FClassFinder<UPlayerMenuWidget> PlayerMenuWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_PlayerMenu.WBP_PlayerMenu_C'"));
    if (PlayerMenuWidgetBPClass.Succeeded())
    {
        PlayerMenuWidgetClass = PlayerMenuWidgetBPClass.Class;
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("PlayerMenuWidget BP is not found."));
        PlayerMenuWidgetClass = UPlayerMenuWidget::StaticClass();
    }

    ItemActionDispatcher = CreateDefaultSubobject<UItemActionDispatcher>(TEXT("ItemActionDispatcher"));
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

        if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
        {
            if (DemoHUD->DemoHUDWidget)
            {
                DemoHUD->DemoHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
            }
        }

        UGameplayStatics::PlaySound2D(this, PlayerMenuOpenSound, 0.5f);
    }
    else // hide
    {
        FInputModeGameOnly InputModeData;
        SetInputMode(InputModeData);

        SetIgnoreLookInput(false);
        SetIgnoreMoveInput(false);
        SetShowMouseCursor(false);

        PlayerMenuWidget->SetVisibilityAndFocus(false);

        // @TODO - Cancel drag

        if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
        {
            if (DemoHUD->DemoHUDWidget)
            {
                DemoHUD->DemoHUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
        }

        UGameplayStatics::PlaySound2D(this, PlayerMenuCloseSound, 0.5f);
    }
}

void ADemoPlayerController::ToggleHelpText()
{
    if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
    {
        if (DemoHUD->DemoHUDWidget)
        {
            DemoHUD->DemoHUDWidget->ToggleHelpText();
        }
    }
}

void ADemoPlayerController::InitDemoHUD()
{
    APawn* OwnerPawn = GetPawn();
    ADemoHUD* DemoHUD = GetHUD<ADemoHUD>();
    if (OwnerPawn && DemoHUD)
    {
        DemoHUD->Init();

        // Bind functions that update HUD widgets.
        if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OwnerPawn))
        {
            PlayerCharacter->OnInteractableFocused.BindUObject(this, &ThisClass::HandleInteractableFocused);
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
        PlayerMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
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