// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DemoInputMappingCollection.generated.h"

class UInputMappingContext;

USTRUCT()
struct FInputMappingContextData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TSoftObjectPtr<UInputMappingContext> InputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    int32 Priority{0};

    // If true, ControlsSectionWidget will register this IMC with the settings to configure key bindings.
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    bool bRegisterWithSettings{true};
};

/**
 * Data asset that holds input mapping contexts used in the game.
 * !!!ControlsSectionWidget identifies a PlayerMappableKeySettings's Name by DemoGameplayTags::Input_* tags.
 * e.g. Input.Jump, Input.Dodge
 * So be sure to set PlayerMappableKeySettings's Name properly in InputAction or IMC.
 */
UCLASS()
class DEMO_API UDemoInputMappingCollection : public UDataAsset
{
    GENERATED_BODY()

public:
    const FInputMappingContextData* GetInputMappingData(FGameplayTag ContextTag) const
    {
        return InputMappingDataMap.Find(ContextTag);
    }

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TMap<FGameplayTag, FInputMappingContextData> InputMappingDataMap;
};