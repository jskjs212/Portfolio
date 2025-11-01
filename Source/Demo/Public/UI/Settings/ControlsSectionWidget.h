// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Settings/SettingsSectionWidget.h"
#include "Framework/Commands/InputChord.h"
#include "GameplayTagContainer.h"
#include "ControlsSectionWidget.generated.h"

class UCheckBox;
class UTagInputKeySelector;

struct FInputKeySelectorData
{
    FGameplayTag InputTag;

    FInputChord CurrentChord;

    TObjectPtr<UTagInputKeySelector> Selector;
};

/**
 *
 */
UCLASS()
class DEMO_API UControlsSectionWidget : public USettingsSectionWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    // Sync UI elements with UserSettings values
    virtual void SyncUIWithUserSettings() override;

private:
    UFUNCTION()
    void HandleWalkInputToggleCheckStateChanged(bool bIsChecked);

    UFUNCTION()
    void HandleSprintInputToggleCheckStateChanged(bool bIsChecked);

    void HandleTagInputKeySelectorKeySelected(FGameplayTag InTag, FInputChord InSelectedKey);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    /* Basics */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCheckBox> WalkInputToggleCheckBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCheckBox> SprintInputToggleCheckBox;

    /**
     * Key bindings
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagInputKeySelector> JumpKeySelector;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTagInputKeySelector> DodgeKeySelector;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    static const FText NoKeySpecifiedText;

    static const TArray<FKey> InputSelectorEscapeKeys;

    TArray<FInputKeySelectorData> InputKeySelectors;
};