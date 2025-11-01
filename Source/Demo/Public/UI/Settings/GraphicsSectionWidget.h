// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Settings/SettingsSectionWidget.h"
#include "GraphicsSectionWidget.generated.h"

namespace ESelectInfo { enum Type : int; }
namespace EWindowAction { enum Type; }
class UComboBoxString;
class FGenericWindow;

/**
 * Consider creating UTagComboBoxString class to leave only one handler for all combo boxes.
 */
UCLASS()
class DEMO_API UGraphicsSectionWidget : public USettingsSectionWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    UGraphicsSectionWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeDestruct() override;

    // Sync UI elements with UserSettings values
    virtual void SyncUIWithUserSettings() override;

    void SyncUIWithResolutionSettings();

    UFUNCTION()
    void HandleDisplayModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void HandleResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void HandleFPSLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void HandleTextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void HandleShadowQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    // Handle when external window action occurs
    // e.g. maximize, restore
    bool HandleWindowAction(const TSharedRef<FGenericWindow>& PlatformWindow, EWindowAction::Type InWindowAction);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> DisplayModeComboBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> ResolutionComboBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> FPSLimitComboBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> TextureQualityComboBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> ShadowQualityComboBox;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    static const FString UnlimitedOption;

    // Resolution options that this game supports.
    // Only hardware-supported resolutions will be added to the combo box.
    static const TArray<FIntPoint> Resolutions;

    static const TArray<FString> QualityOptions;

    // Window action delegate
    FOnWindowAction OnWindowActionDelegate;
    FDelegateHandle OnWindowActionDelegateHandle;
};