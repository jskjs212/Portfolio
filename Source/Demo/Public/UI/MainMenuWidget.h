// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UBorder;
class UButton;
class USettingsPageWidget;
class UVerticalBox;

/**
 *
 */
UCLASS()
class DEMO_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
public:
    UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;

    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
private:
    void InitSettingsPage();

    void ShowSettingsPage(bool bShow);

    UFUNCTION()
    void HandleNewGameButtonClicked();

    UFUNCTION()
    void HandleSettingsButtonClicked() { ShowSettingsPage(true); }

    UFUNCTION()
    void HandleQuitButtonClicked();

    UFUNCTION()
    void HandleSettingsPageCloseButtonClicked() { ShowSettingsPage(false); }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> MainMenuVerticalBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> NewGameButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> LoadButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SettingsButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> SettingsPageBorder;

    // Child of SettingsPageBorder, contains SettingsPageWidget.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> SettingsPageVerticalBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SettingsPageCloseButton;

    TSubclassOf<USettingsPageWidget> SettingsPageWidgetClass;

    TObjectPtr<USettingsPageWidget> SettingsPageWidget;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FName StartLevelName;
};