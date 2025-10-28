// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsSectionWidget.generated.h"

/**
 * Base class for settings section widgets.
 * e.g. Graphics, Audio, etc.
 *
 * @TODO - ApplySettings(), ResetToDefaults(), CancelChanges()
 */
UCLASS()
class DEMO_API USettingsSectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void SyncUIWithUserSettings() {}
};