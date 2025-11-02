// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputKeySelector.h"
#include "DemoInputKeySelector.generated.h"

DECLARE_DELEGATE_OneParam(FOnDemoKeySelected, FInputChord /* InSelectedKey */);
DECLARE_DELEGATE(FOnDemoIsSelectingKeyChanged);

/**
 * Input key selector with static delegates that can handle with payload.
 */
UCLASS()
class DEMO_API UDemoInputKeySelector : public UInputKeySelector
{
    GENERATED_BODY()

public:
    FOnDemoKeySelected OnDemoKeySelected;
    FOnDemoIsSelectingKeyChanged OnDemoIsSelectingKeyChanged;

protected:
    virtual void OnWidgetRebuilt() override
    {
        Super::OnWidgetRebuilt();

        OnKeySelected.AddDynamic(this, &ThisClass::HandleDemoKeySelected);
        OnIsSelectingKeyChanged.AddDynamic(this, &ThisClass::HandleDemoIsSelectingKeyChanged);
    }

private:
    UFUNCTION()
    void HandleDemoKeySelected(FInputChord InSelectedKey)
    {
        OnDemoKeySelected.ExecuteIfBound(InSelectedKey);
    }

    UFUNCTION()
    void HandleDemoIsSelectingKeyChanged()
    {
        OnDemoIsSelectingKeyChanged.ExecuteIfBound();
    }
};
