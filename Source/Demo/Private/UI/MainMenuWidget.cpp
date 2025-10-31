// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerController/StartUpPlayerController.h"
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

    checkf(MainMenuVerticalBox && NewGameButton && LoadButton && SettingsButton && QuitButton
        && SettingsPageBorder && SettingsPageVerticalBox && SettingsPageCloseButton,
        TEXT("Failed to bind widgets."));

    LoadButton->SetIsEnabled(false);
    SettingsPageBorder->SetVisibility(ESlateVisibility::Collapsed);
    SettingsPageCloseButton->SetVisibility(ESlateVisibility::Collapsed);

    NewGameButton->OnClicked.AddDynamic(this, &ThisClass::HandleNewGameButtonClicked);
    SettingsButton->OnClicked.AddDynamic(this, &ThisClass::HandleSettingsButtonClicked);
    QuitButton->OnClicked.AddDynamic(this, &ThisClass::HandleQuitButtonClicked);
    SettingsPageCloseButton->OnClicked.AddDynamic(this, &ThisClass::HandleSettingsPageCloseButtonClicked);
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
    SettingsPageWidget->SyncUIWithUserSettings();
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

void UMainMenuWidget::HandleNewGameButtonClicked()
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

void UMainMenuWidget::HandleQuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}