// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Settings/ControlsSectionWidget.h"
#include "Character/PlayerCharacter.h"
#include "Components/CheckBox.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "Settings/DemoUserSettings.h"
#include "UI/Components/TagInputKeySelector.h"

// Static variable initialization - @hardcoded
const FText UControlsSectionWidget::NoKeySpecifiedText{FText::FromString(TEXT("-"))};
const TArray<FKey> UControlsSectionWidget::InputSelectorEscapeKeys{
    EKeys::Gamepad_Special_Right,
    EKeys::Escape,
    EKeys::Tab
};

void UControlsSectionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(WalkInputToggleCheckBox && SprintInputToggleCheckBox
        && JumpKeySelector && DodgeKeySelector,
        TEXT("Failed to bind widgets."));

    // Initialize entries
    InputKeySelectors = {
        {DemoGameplayTags::Input_Jump, FInputChord{}, JumpKeySelector},
        {DemoGameplayTags::Input_Dodge, FInputChord{}, DodgeKeySelector}
    };

    // Configure widgets
    for (FInputKeySelectorData& Data : InputKeySelectors)
    {
        Data.Selector->SetEscapeKeys(InputSelectorEscapeKeys);
        Data.Selector->SetNoKeySpecifiedText(NoKeySpecifiedText);
        Data.Selector->SetAllowModifierKeys(false); // @TODO @TEST - temporary
        Data.Selector->SetTypeTag(Data.InputTag);
    }

    SyncUIWithUserSettings();

    // Bind events
    WalkInputToggleCheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::HandleWalkInputToggleCheckStateChanged);
    SprintInputToggleCheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::HandleSprintInputToggleCheckStateChanged);

    for (FInputKeySelectorData& Data : InputKeySelectors)
    {
        Data.Selector->OnTagInputKeySelectorKeySelected.BindUObject(this, &ThisClass::HandleTagInputKeySelectorKeySelected);
    }
}

void UControlsSectionWidget::SyncUIWithUserSettings()
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        // Basics
        WalkInputToggleCheckBox->SetIsChecked(UserSettings->GetWalkInputToggle());
        SprintInputToggleCheckBox->SetIsChecked(UserSettings->GetSprintInputToggle());

        // Key bindings
        if (APlayerController* PlayerController = GetOwningPlayer())
        {
            APlayerCharacter* PlayerCharacter = PlayerController->GetPawn<APlayerCharacter>();
            for (FInputKeySelectorData& Data : InputKeySelectors)
            {
                const FInputChord* SelectedKey = UserSettings->GetInputKey(Data.InputTag);
                if (SelectedKey)
                {
                    Data.Selector->SetSelectedKey(*SelectedKey);
                }
                // @TODO @TEST - MainMenu
                if (!PlayerCharacter)
                {
                    Data.Selector->SetIsEnabled(false);
                }
            }
        }
    }
}

void UControlsSectionWidget::HandleWalkInputToggleCheckStateChanged(bool bIsChecked)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        UserSettings->SetWalkInputToggle(bIsChecked);
        UserSettings->ApplySettings(false);
    }
}

void UControlsSectionWidget::HandleSprintInputToggleCheckStateChanged(bool bIsChecked)
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        UserSettings->SetSprintInputToggle(bIsChecked);
        UserSettings->ApplySettings(false);
    }
}

void UControlsSectionWidget::HandleTagInputKeySelectorKeySelected(FGameplayTag InTag, FInputChord InSelectedKey)
{
    UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings();
    if (!UserSettings)
    {
        return;
    }


    APlayerController* PlayerController = GetOwningPlayer();
    if (!PlayerController)
    {
        return;
    }

    // @TODO @TEST - In main menu, there's no PlayerCharacter.
    // -> Need to figure out a better way to check key conflicts.

    // @TODO - To check if the key is already bound, directly accessing to the PlayerCharacter. How can I decouple this?
    // DemoUserSettings::SetKey -> delegate -> PlayerCharacter::rebind can't echo the key conflict to the Settings UI.
    APlayerCharacter* PlayerCharacter = PlayerController->GetPawn<APlayerCharacter>();
    if (!PlayerCharacter)
    {
        return;
    }

    // @TODO @TEST
    bool bIsAlreadyBound = PlayerCharacter->IsInputKeyBound(InSelectedKey);
    if (bIsAlreadyBound)
    {
        // Revert the selection to previous key
        const FInputChord* PreviousKey = UserSettings->GetInputKey(InTag);
        if (PreviousKey)
        {
            for (FInputKeySelectorData& Data : InputKeySelectors)
            {
                if (Data.InputTag == InTag)
                {
                    Data.Selector->SetSelectedKey(*PreviousKey);
                    break;
                }
            }
        }
    }
    else // Not bound
    {
        UserSettings->SetInputKey(InTag, InSelectedKey);
        UserSettings->ApplySettings(false);
    }
}