// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SystemMenuPageWidget.generated.h"

class UButton;

/**
 *
 */
UCLASS()
class DEMO_API USystemMenuPageWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
public:
    USystemMenuPageWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeDestruct() override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
private:
    UFUNCTION()
    void HandleContinueButtonClicked();

    UFUNCTION()
    void HandleSaveButtonClicked();

    void HandleAsyncSaveCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess);

    UFUNCTION()
    void HandleLoadButtonClicked();

    void HandleLoadedSaveGameDiscarded();

    UFUNCTION()
    void HandleExitToMainMenuButtonClicked();

    UFUNCTION()
    void HandleExitToDesktopButtonClicked();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ContinueButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SaveButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> LoadButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ExitToMainMenuButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ExitToDesktopButton;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    FAsyncSaveGameToSlotDelegate AsyncSavedDelegate;
};