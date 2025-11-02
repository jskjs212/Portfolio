// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Settings/ControlsSectionWidget.h"
#include "Components/CheckBox.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "GameplayTagContainer.h"
#include "Input/DemoInputMappingCollection.h"
#include "Input/DemoInputUserSettings.h"
#include "InputMappingContext.h"
#include "Settings/DemoProjectSettings.h"
#include "Settings/DemoUserSettings.h"
#include "UI/Components/DemoInputKeySelector.h"
#include "UserSettings/EnhancedInputUserSettings.h"

// Static variable initialization - @hardcoded
const FText UControlsSectionWidget::NoKeySpecifiedText{FText::FromString(TEXT("None"))};
const TArray<FKey> UControlsSectionWidget::InputSelectorEscapeKeys{
    EKeys::Gamepad_Special_Right,
    EKeys::Escape,
    EKeys::Tab
};

void UControlsSectionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(WalkInputToggleCheckBox && SprintInputToggleCheckBox
        && WalkKeySelector && SprintKeySelector && JumpKeySelector && DodgeKeySelector,
        TEXT("Failed to bind widgets."));

    // Initialize entries
    InputKeySelectorMap = {
        {DemoGameplayTags::Input_Walk.GetTag().GetTagName(),   {FInputChord{}, WalkKeySelector}},
        {DemoGameplayTags::Input_Sprint.GetTag().GetTagName(), {FInputChord{}, SprintKeySelector}},
        {DemoGameplayTags::Input_Jump.GetTag().GetTagName(),   {FInputChord{}, JumpKeySelector}},
        {DemoGameplayTags::Input_Dodge.GetTag().GetTagName(),  {FInputChord{}, DodgeKeySelector}}
    };

    // Configure widgets and set MappingNames
    for (auto& [MappingName, Data] : InputKeySelectorMap)
    {
        Data.Selector->SetEscapeKeys(InputSelectorEscapeKeys);
        Data.Selector->SetNoKeySpecifiedText(NoKeySpecifiedText);
        Data.Selector->SetAllowModifierKeys(false); // @TODO - modifiers
    }

    SyncUIWithUserSettings();
    RegisterAllInputMappingContexts();
    SyncUIWithEnhancedInputUserSettings();

    // Bind events
    WalkInputToggleCheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::HandleWalkInputToggleCheckStateChanged);
    SprintInputToggleCheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::HandleSprintInputToggleCheckStateChanged);

    for (auto& [MappingName, Data] : InputKeySelectorMap)
    {
        Data.Selector->OnDemoKeySelected.BindUObject(this, &ThisClass::HandleDemoInputKeySelectorKeySelected, MappingName);
    }
}

void UControlsSectionWidget::SyncUIWithUserSettings()
{
    if (UDemoUserSettings* UserSettings = UDemoUserSettings::GetDemoUserSettings())
    {
        // Basics
        WalkInputToggleCheckBox->SetIsChecked(UserSettings->GetWalkInputToggle());
        SprintInputToggleCheckBox->SetIsChecked(UserSettings->GetSprintInputToggle());
    }
}

void UControlsSectionWidget::SyncUIWithEnhancedInputUserSettings()
{
    const UEnhancedInputUserSettings* EIUserSettings = UDemoInputUserSettings::GetUserSettingsForLocalPlayer(GetOwningLocalPlayer());
    if (!EIUserSettings)
    {
        return;
    }

    // Assume only the first slot was used for MapPlayerKey().
    constexpr EPlayerMappableKeySlot FirstSlot = EPlayerMappableKeySlot::First;
    for (auto& [MappingName, Data] : InputKeySelectorMap)
    {
        // Find the current mapping from active profile
        const FPlayerKeyMapping* PlayerKeyMapping = EIUserSettings->FindCurrentMappingForSlot(MappingName, FirstSlot);
        if (PlayerKeyMapping)
        {
            // Update UI
            // @TODO - Modifiers
            Data.CurrentChord.Key = PlayerKeyMapping->GetCurrentKey();
            Data.Selector->SetSelectedKey(Data.CurrentChord);
        }
    }
}

void UControlsSectionWidget::RegisterAllInputMappingContexts()
{
    UEnhancedInputUserSettings* EIUserSettings = UDemoInputUserSettings::GetUserSettingsForLocalPlayer(GetOwningLocalPlayer());
    if (!EIUserSettings)
    {
        return;
    }

    const UDemoInputMappingCollection* InputMappingCollection = GetDefault<UDemoProjectSettings>()->InputMappingCollection.LoadSynchronous();
    if (!InputMappingCollection)
    {
        DemoLOG_CF(LogInput, Error, TEXT("InputMappingCollection is not set in DemoProjectSettings."));
        return;
    }

    // Register all IMCs in the collection with the EIUserSettings
    for (const auto& [ContextTag, InputMappingData] : InputMappingCollection->InputMappingDataMap)
    {
        // Skip if not required
        if (!InputMappingData.bRegisterWithSettings)
        {
            continue;
        }

        const UInputMappingContext* InputMappingContext = InputMappingData.InputMappingContext.LoadSynchronous();
        if (InputMappingContext)
        {
            EIUserSettings->RegisterInputMappingContext(InputMappingContext);
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

void UControlsSectionWidget::HandleDemoInputKeySelectorKeySelected(FInputChord InSelectedKey, FName InMappingName)
{
    FInputKeySelectorData& SelectorData = InputKeySelectorMap[InMappingName];
    if (InSelectedKey == SelectorData.CurrentChord)
    {
        // No change
        return;
    }

    UEnhancedInputUserSettings* EIUserSettings = UDemoInputUserSettings::GetUserSettingsForLocalPlayer(GetOwningLocalPlayer());
    if (!EIUserSettings)
    {
        return;
    }

    UEnhancedPlayerMappableKeyProfile* ActiveKeyProfile = EIUserSettings->GetActiveKeyProfile();
    if (!ActiveKeyProfile)
    {
        return;
    }

    // Check for conflicts
    TArray<FName> ConflictingMappings;
    ActiveKeyProfile->GetMappingNamesForKey(InSelectedKey.Key, ConflictingMappings);

    // No conflicts, or None (unbound)
    if (ConflictingMappings.IsEmpty() || InSelectedKey.Key == EKeys::Invalid)
    {
        FMapPlayerKeyArgs Args;
        Args.MappingName = InMappingName;
        Args.NewKey = InSelectedKey.Key;
        Args.Slot = EPlayerMappableKeySlot::First; // Only one slot for now

        FGameplayTagContainer FailureReason;
        EIUserSettings->MapPlayerKey(Args, FailureReason);

        if (FailureReason.IsEmpty())
        {
            // Successfully bound -> return
            DemoLOG_CF(LogInput, Verbose, TEXT("Successfully bound. Key: %s, Action: %s"),
                *InSelectedKey.Key.ToString(),
                *InMappingName.ToString()
            );
            SelectorData.CurrentChord = InSelectedKey;
            EIUserSettings->AsyncSaveSettings();
            return;
        }
        else
        {
            // Failed to bind -> revert
            DemoLOG_CF(LogInput, Warning, TEXT("Failed to bind key: %s, Action: %s, Reason: %s"),
                *InSelectedKey.Key.ToString(),
                *InMappingName.ToString(),
                *FailureReason.ToStringSimple()
            );
        }
    }
    else
    {
        // Already bound -> revert
        for (const FName& MappingName : ConflictingMappings)
        {
            // @TODO - Show warning message with conflicting action names.
            DemoLOG_CF(LogInput, Error, TEXT("Already bound. Key: %s. Bound action: %s"),
                *InSelectedKey.Key.ToString(),
                *MappingName.ToString()
            );
        }
    }

    // Failed to bind. Revert the selection to previous key.
    // @WARNING - This fires the OnKeySelected again, unless you create a custom selector widget.
    SelectorData.Selector->SetSelectedKey(SelectorData.CurrentChord);
}