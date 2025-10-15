// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DemoInputConfig.generated.h"

class UInputAction;

/**
 * Struct that pairs a GameplayTag with an InputAction.
 */
USTRUCT()
struct FDemoInputAction
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Input"))
    FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<const UInputAction> InputAction;
};

/**
 * Input configuration data asset.
 */
UCLASS(Const)
class DEMO_API UDemoInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    const UInputAction* FindInputActionForTag(FGameplayTag InputTag) const;

protected:
    UPROPERTY(EditDefaultsOnly)
    TArray<FDemoInputAction> InputActions;
};