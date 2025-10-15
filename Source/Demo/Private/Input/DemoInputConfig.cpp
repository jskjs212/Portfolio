// Fill out your copyright notice in the Description page of Project Settings.

#include "Input/DemoInputConfig.h"
#include "DemoTypes/LogCategories.h"

const UInputAction* UDemoInputConfig::FindInputActionForTag(FGameplayTag InputTag) const
{
    for (const FDemoInputAction& Action : InputActions)
    {
        if (Action.InputTag == InputTag)
        {
            return Action.InputAction;
        }
    }

    DemoLOG_CF(LogDemoGame, Error, TEXT("Failed to find InputAction for Tag: %s (on %s)"), *InputTag.ToString(), *GetFullName());
    return nullptr;
}