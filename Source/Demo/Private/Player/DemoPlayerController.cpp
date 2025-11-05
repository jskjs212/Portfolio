// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/DemoPlayerController.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/PlayerCharacter.h"
#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoTypes.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/UITypes.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/DemoInputConfig.h"
#include "Input/DemoInputHelper.h"
#include "Kismet/GameplayStatics.h"
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

    static ConstructorHelpers::FClassFinder<UUserWidget> BossVictoryWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/Combat/WBP_BossVictory.WBP_BossVictory_C'"));
    if (BossVictoryWidgetBPClass.Succeeded())
    {
        BossVictoryWidgetClass = BossVictoryWidgetBPClass.Class;
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("BossVictoryWidget BP is not found."));
        BossVictoryWidgetClass = UUserWidget::StaticClass();
    }

    static ConstructorHelpers::FClassFinder<UUserWidget> YouDiedWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/Combat/WBP_YouDied.WBP_YouDied_C'"));
    if (YouDiedWidgetBPClass.Succeeded())
    {
        YouDiedWidgetClass = YouDiedWidgetBPClass.Class;
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("YouDiedWidget BP is not found."));
        YouDiedWidgetClass = UUserWidget::StaticClass();
    }

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

    ItemActionDispatcher = CreateDefaultSubobject<UItemActionDispatcher>(TEXT("ItemActionDispatcher"));
}

void ADemoPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (MainMenuLevelName == NAME_None)
    {
        DemoLOG_CF(LogUI, Error, TEXT("MainMenuLevelName is not set."));
    }

    SetupPlayerInput();

    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->OnDeath.AddUObject(this, &ThisClass::HandleCharacterDeath);
    }

    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
#if WITH_EDITOR
        // Lazy load for editor
        AudioSubsystem->LoadUserAudioSettings();
#endif // WITH_EDITOR

        AudioSubsystem->PlayDefaultMusic(this);
    }
}

void ADemoPlayerController::ShowPlayerMenu(bool bShow, FGameplayTag TabTag)
{
    if (bShow)
    {
        // Input mode
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(PlayerMenuWidget->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
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

void ADemoPlayerController::ShowBossVictoryWidget()
{
    if (!BossVictoryWidget)
    {
        BossVictoryWidget = CreateWidget<UUserWidget>(this, BossVictoryWidgetClass);
    }
    BossVictoryWidget->AddToViewport(Demo::ZOrder::Event_BossVictory);

    // Hide BossAIStatus
    ShowBossAIStatus(nullptr);

    // Prepare BossVictoryWidget remove after duration
    if (UWorld* World = GetWorld())
    {
        FTimerHandle BossVictoryWidgetTimerHandle;
        FTimerDelegate BossVictoryWidgetTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::HideBossVictoryWidget);

        World->GetTimerManager().SetTimer(BossVictoryWidgetTimerHandle, BossVictoryWidgetTimerDelegate, BossVictoryWidgetDuration, false);
    }

    // Play sound
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(this, DemoSoundTags::SFX_Combat_Boss_Victory);
    }
}

void ADemoPlayerController::HideBossVictoryWidget()
{
    if (BossVictoryWidget)
    {
        // @misc - Might be reused later. Does it need to be nullptr?
        BossVictoryWidget->RemoveFromParent();
    }
}

void ADemoPlayerController::ShowYouDiedWidgetAndAddAfterDeathInputContext()
{
    if (!YouDiedWidget)
    {
        YouDiedWidget = CreateWidget<UUserWidget>(this, YouDiedWidgetClass);
    }
    YouDiedWidget->AddToViewport(Demo::ZOrder::Event_YouDied);

    // Add after-death IMC to subsystem
    if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        const auto [AfterDeathMappingContext, Priority] = Demo::Input::FindInputMappingContext(AfterDeathInputMappingContextTag);
        if (AfterDeathMappingContext)
        {
            EISubsystem->AddMappingContext(AfterDeathMappingContext, Priority);
        }
        else
        {
            DemoLOG_CF(LogInput, Error, TEXT("Failed to add AfterDeathMappingContext."));
        }
    }

    // Play sound
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySound2D(this, DemoSoundTags::SFX_Combat_YouDied);
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

void ADemoPlayerController::GoToMainMenu()
{
    FInputModeUIOnly InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
    SetInputMode(InputModeData);
    SetShowMouseCursor(true);

    UGameplayStatics::OpenLevel(this, MainMenuLevelName);
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
            PlayerMenuWidget->AddToViewport(Demo::ZOrder::PlayerMenu);
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

void ADemoPlayerController::HandleCharacterDeath()
{
    // Prepare YouDiedWidget display after delay
    if (UWorld* World = GetWorld())
    {
        FTimerHandle YouDiedWidgetTimerHandle;
        FTimerDelegate YouDiedWidgetTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ShowYouDiedWidgetAndAddAfterDeathInputContext);

        World->GetTimerManager().SetTimer(YouDiedWidgetTimerHandle, YouDiedWidgetTimerDelegate, YouDiedWidgetDelay, false);
    }
}

void ADemoPlayerController::HandleBossDeath()
{
    // Prepare BossVictoryWidget display after delay
    if (UWorld* World = GetWorld())
    {
        FTimerHandle BossVictoryWidgetTimerHandle;
        FTimerDelegate BossVictoryWidgetTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::ShowBossVictoryWidget);

        World->GetTimerManager().SetTimer(BossVictoryWidgetTimerHandle, BossVictoryWidgetTimerDelegate, BossVictoryWidgetDelay, false);
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

void ADemoPlayerController::SetupPlayerInput()
{
    if (!DefaultInputMappingContextTag.IsValid() || !AfterDeathInputMappingContextTag.IsValid() || !PlayerControllerInputConfig)
    {
        DemoLOG_CF(LogInput, Error, TEXT("Initialization|Input variables are not set properly."));
    }

    // Add default IMC to subsystem
    if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        const auto [DefaultMappingContext, Priority] = Demo::Input::FindInputMappingContext(DefaultInputMappingContextTag);
        if (DefaultMappingContext)
        {
            EISubsystem->AddMappingContext(DefaultMappingContext, Priority);
        }
        else
        {
            DemoLOG_CF(LogInput, Error, TEXT("Failed to add DefaultMappingContext."));
        }
    }

    // Bind input actions
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Helper lambda
        auto BindAction = [this, EnhancedInputComponent](FGameplayTag InputTag, ETriggerEvent TriggerEvent, void(ThisClass::* Func)())
            {
                if (const UInputAction* InputAction = PlayerControllerInputConfig->FindInputActionForTag(InputTag))
                {
                    EnhancedInputComponent->BindAction(InputAction, TriggerEvent, this, Func);
                }
                else
                {
                    DemoLOG_CF(LogDemoGame, Error, TEXT("InputAction not found for tag %s"), *InputTag.ToString());
                }
            };

        BindAction(DemoGameplayTags::Input_GoToMainMenu, ETriggerEvent::Started, &ThisClass::GoToMainMenu);
        BindAction(DemoGameplayTags::Input_ShowPlayerMenu, ETriggerEvent::Started, &ThisClass::ShowPlayerMenuActionStarted);
        BindAction(DemoGameplayTags::Input_ToggleHelpText, ETriggerEvent::Started, &ThisClass::ToggleHelpText);
        BindAction(DemoGameplayTags::Input_Escape, ETriggerEvent::Started, &ThisClass::EscapeActionStarted);
    }
}

void ADemoPlayerController::ShowPlayerMenuActionStarted()
{
    ABaseCharacter* BaseCharacter = GetPawn<ABaseCharacter>();
    if (BaseCharacter && !BaseCharacter->IsDead())
    {
        ShowPlayerMenu(true);
    }
}

void ADemoPlayerController::EscapeActionStarted()
{
    ABaseCharacter* BaseCharacter = GetPawn<ABaseCharacter>();
    if (BaseCharacter && !BaseCharacter->IsDead())
    {
        ShowPlayerMenu(true, DemoGameplayTags::UI_PlayerMenu_SystemMenu);
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