// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsSectionWidget.generated.h"

/**
 * Base class for settings section widgets.
 * e.g. Graphics, Audio, etc.
 *
 * Apply settings when UI elements change for now.
 * @TODO - ApplySettings(), ResetToDefaults(), CancelChanges(), MarkDirty(), etc.
 */
UCLASS()
class DEMO_API USettingsSectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void SyncUIWithUserSettings() {}
};