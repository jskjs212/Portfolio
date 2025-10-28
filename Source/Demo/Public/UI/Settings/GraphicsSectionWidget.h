// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Settings/SettingsSectionWidget.h"
#include "GraphicsSectionWidget.generated.h"

namespace ESelectInfo { enum Type : int; }
class UComboBoxString;

/**
 *
 */
UCLASS()
class DEMO_API UGraphicsSectionWidget : public USettingsSectionWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

    virtual void SyncUIWithUserSettings() override;

    UFUNCTION()
    void HandleFPSLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> FPSLimitComboBox;

    static inline const FString UnlimitedOption{TEXT("Unlimited")};
};