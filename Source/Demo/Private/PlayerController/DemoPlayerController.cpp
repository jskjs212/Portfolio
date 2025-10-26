// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/DemoPlayerController.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/PlayerCharacter.h"
#include "DemoTypes/LogCategories.h"
#include "UI/CursorWidget.h"
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

    static ConstructorHelpers::FClassFinder<UCursorWidget> CursorWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/Cursors/WBP_Cursor.WBP_Cursor_C'"));
    if (CursorWidgetBPClass.Succeeded())
    {
        CursorWidgetClass = CursorWidgetBPClass.Class;
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("CursorWidget BP is not found."));
        CursorWidgetClass = UCursorWidget::StaticClass();
    }

    ItemActionDispatcher = CreateDefaultSubobject<UItemActionDispatcher>(TEXT("ItemActionDispatcher"));
}

void ADemoPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlayDefaultBGM(this);
    }
}

void ADemoPlayerController::ShowPlayerMenu(bool bShow, FGameplayTag TabTag)
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
        if (TabTag.IsValid())
        {
            PlayerMenuWidget->SelectTab(TabTag);
        }

        if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
        {
            if (DemoHUD->DemoHUDWidget)
            {
                DemoHUD->DemoHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
            }
        }

        if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
        {
            AudioSubsystem->PlaySound2D(this, DemoSoundTags::UI_PlayerMenu_Open);
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

        // @TODO - Cancel drag

        if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
        {
            if (DemoHUD->DemoHUDWidget)
            {
                DemoHUD->DemoHUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
        }

        if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
        {
            AudioSubsystem->PlaySound2D(this, DemoSoundTags::UI_PlayerMenu_Close);
        }
    }
}

void ADemoPlayerController::ShowBossAIStatus(AActor* BossActor)
{
    if (ADemoHUD* DemoHUD = GetHUD<ADemoHUD>())
    {
        if (DemoHUD->DemoHUDWidget)
        {
            DemoHUD->DemoHUDWidget->ShowBossAIStatus(BossActor);
        }
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

void ADemoPlayerController::SetCursorState(ECursorState NewCursorState)
{
    if (NewCursorState != CursorState)
    {
        // @hardcoded - Only one exception for now.
        if (UWidgetBlueprintLibrary::IsDragDropping() && NewCursorState != ECursorState::Dragging)
        {
            // Can't change to other state while dragging.
            return;
        }

        CursorState = NewCursorState;
        CursorWidget->UpdateCursorVisuals(CursorState);
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
    if (PlayerMenuWidgetClass)
    {
        PlayerMenuWidget = CreateWidget<UPlayerMenuWidget>(this, PlayerMenuWidgetClass);
        if (PlayerMenuWidget)
        {
            PlayerMenuWidget->InitPlayerMenu();
            PlayerMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
            PlayerMenuWidget->AddToViewport(1);
        }
    }
}

void ADemoPlayerController::InitCursor()
{
    if (CursorWidgetClass)
    {
        CursorWidget = CreateWidget<UCursorWidget>(this, CursorWidgetClass);
        if (CursorWidget)
        {
            SetMouseCursorWidget(EMouseCursor::Default, CursorWidget);
        }
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