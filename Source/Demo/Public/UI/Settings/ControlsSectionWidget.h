// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Settings/SettingsSectionWidget.h"
#include "Framework/Commands/InputChord.h"
#include "ControlsSectionWidget.generated.h"

class UCheckBox;
class UEnhancedInputUserSettings;
class UDemoInputKeySelector;

struct FInputKeySelectorData
{
    FInputChord CurrentChord;

    TObjectPtr<UDemoInputKeySelector> Selector;
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

    void SyncUIWithEnhancedInputUserSettings();

private:
    // Register all IMCs from project settings with EIUserSettings to enable key binding configuration. (Don't activate)
    void RegisterAllInputMappingContexts();

    UFUNCTION()
    void HandleWalkInputToggleCheckStateChanged(bool bIsChecked);

    UFUNCTION()
    void HandleSprintInputToggleCheckStateChanged(bool bIsChecked);

    // If there is no conflict, bind the new key to the InputAction using EIUserSettings::MapPlayerKey.
    // @TODO - modifiers
    // @TODO - Handle with the keys that are used for multiple actions. Once changed, can't use the same key anymore.
    void HandleDemoInputKeySelectorKeySelected(FInputChord InSelectedKey, FName InMappingName);

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
    TObjectPtr<UDemoInputKeySelector> WalkKeySelector;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDemoInputKeySelector> SprintKeySelector;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDemoInputKeySelector> JumpKeySelector;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDemoInputKeySelector> DodgeKeySelector;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    static const FText NoKeySpecifiedText;

    static const TArray<FKey> InputSelectorEscapeKeys;

    // {MappingName, {CurrentChord, Selector}}
    TMap<FName, FInputKeySelectorData> InputKeySelectorMap;
};