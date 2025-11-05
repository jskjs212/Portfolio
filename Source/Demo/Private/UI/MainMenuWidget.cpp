// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/StartUpPlayerController.h"
#include "Settings/DemoSaveGame.h"
#include "System/DemoGameInstance.h"
#include "UI/SettingsPageWidget.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // @hardcoded
    static ConstructorHelpers::FClassFinder<USettingsPageWidget> SettingsPageWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/Settings/WBP_SettingsPage.WBP_SettingsPage_C'"));
    if (SettingsPageWidgetBPClass.Succeeded())
    {
        SettingsPageWidgetClass = SettingsPageWidgetBPClass.Class;
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("SettingsPageWidget BP is not found."));
        SettingsPageWidgetClass = USettingsPageWidget::StaticClass();
    }
}

void UMainMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIsFocusable(true);

    checkf(MainMenuVerticalBox && NewGameButton && LoadGameButton && SettingsButton && QuitButton
        && SettingsPageBorder && SettingsPageVerticalBox && SettingsPageCloseButton,
        TEXT("Failed to bind widgets."));

    bool bSaveExists = UGameplayStatics::DoesSaveGameExist(
        UDemoSaveGame::DefaultSaveSlot, UDemoSaveGame::DefaultUserIndex);
    LoadGameButton->SetIsEnabled(bSaveExists);

    SettingsPageBorder->SetVisibility(ESlateVisibility::Collapsed);
    SettingsPageCloseButton->SetVisibility(ESlateVisibility::Collapsed);

    NewGameButton->OnClicked.AddDynamic(this, &ThisClass::HandleNewGameButtonClicked);
    LoadGameButton->OnClicked.AddDynamic(this, &ThisClass::HandleLoadGameButtonClicked);
    SettingsButton->OnClicked.AddDynamic(this, &ThisClass::HandleSettingsButtonClicked);
    QuitButton->OnClicked.AddDynamic(this, &ThisClass::HandleQuitButtonClicked);
    SettingsPageCloseButton->OnClicked.AddDynamic(this, &ThisClass::HandleSettingsPageCloseButtonClicked);

    if (UDemoGameInstance* DemoGameInstance = GetGameInstance<UDemoGameInstance>())
    {
        DemoGameInstance->OnLoadedSaveGameDiscarded.AddUObject(this, &ThisClass::HandleLoadedSaveGameDiscarded);
    }
}

FReply UMainMenuWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Key(Tab) -> close settings page
    if (InKeyEvent.GetKey() == EKeys::Tab)
    {
        if (SettingsPageBorder->GetVisibility() != ESlateVisibility::Collapsed)
        {
            ShowSettingsPage(false);
            return FReply::Handled();
        }
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UMainMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Key(Escape) -> close settings page
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        if (SettingsPageBorder->GetVisibility() != ESlateVisibility::Collapsed)
        {
            ShowSettingsPage(false);
            return FReply::Handled();
        }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UMainMenuWidget::InitSettingsPage()
{
    if (!SettingsPageWidgetClass)
    {
        DemoLOG_CF(LogUI, Error, TEXT("SettingsPageWidgetClass is not set."));
        return;
    }

    SettingsPageWidget = CreateWidget<USettingsPageWidget>(GetOwningPlayer(), SettingsPageWidgetClass);
    SettingsPageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    SettingsPageVerticalBox->AddChild(SettingsPageWidget);
}

void UMainMenuWidget::ShowSettingsPage(bool bShow)
{
    if (!SettingsPageWidget)
    {
        InitSettingsPage();
        if (!SettingsPageWidget)
        {
            DemoLOG_CF(LogUI, Error, TEXT("Failed to create SettingsPageWidget."));
            return;
        }
    }

    MainMenuVerticalBox->SetVisibility(bShow ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
    SettingsPageBorder->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    SettingsPageCloseButton->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UMainMenuWidget::TryStartNewGame()
{
    if (AStartUpPlayerController* StartUpPlayerController = GetOwningPlayer<AStartUpPlayerController>())
    {
        StartUpPlayerController->StartNewGame();
    }
    else
    {
        DemoLOG_CF(LogUI, Error, TEXT("OwningPlayer is not AStartUpPlayerController."));
    }
}

void UMainMenuWidget::HandleNewGameButtonClicked()
{
    TryStartNewGame();
}

void UMainMenuWidget::HandleLoadGameButtonClicked()
{
    UDemoGameInstance* DemoGameInstance = GetGameInstance<UDemoGameInstance>();
    if (!DemoGameInstance)
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to get DemoGameInstance."));
    }

    const FString& SlotName = UDemoSaveGame::DefaultSaveSlot;
    const int32 UserIndex = UDemoSaveGame::DefaultUserIndex;
    UDemoSaveGame* LoadedSaveGame = Cast<UDemoSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
    if (!LoadedSaveGame)
    {
        UE_LOG(LogDemoGame, Warning, TEXT("Failed to load game from slot: %s, UserIndex: %d"),
            *SlotName, UserIndex);
        return;
    }

    if (!DemoGameInstance->LoadedSaveGameVersionCheck(LoadedSaveGame))
    {
        return;
    }

    DemoGameInstance->RequestLoadGame(LoadedSaveGame); // Just request, actual applying happens after map load.

    // @TODO - Start from saved map
    TryStartNewGame();
}

void UMainMenuWidget::HandleLoadedSaveGameDiscarded()
{
    LoadGameButton->SetIsEnabled(false);
}

void UMainMenuWidget::HandleQuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}