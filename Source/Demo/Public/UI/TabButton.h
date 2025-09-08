// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "GameplayTagContainer.h"
#include "TabButton.generated.h"

DECLARE_DELEGATE_OneParam(FOnTabButtonClicked, FGameplayTag /* InTag */);
DECLARE_DELEGATE_OneParam(FOnTabButtonHovered, FGameplayTag /* InTag */);
DECLARE_DELEGATE_OneParam(FOnTabButtonUnhovered, FGameplayTag /* InTag */);

/**
 * Tab button that has a tab type.
 * When button events are triggered, the owner of this button can handle them with the tab type in generic way.
 */
UCLASS()
class DEMO_API UTabButton : public UButton
{
    GENERATED_BODY()

public:
    FOnTabButtonClicked OnTabButtonClicked;
    FOnTabButtonHovered OnTabButtonHovered;
    FOnTabButtonUnhovered OnTabButtonUnhovered;

    virtual void SynchronizeProperties() override
    {
        Super::SynchronizeProperties();

        if (!TabTag.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("TabTag is not set in %s"), *GetName());
        }

        // TEST:
        PRAGMA_DISABLE_DEPRECATION_WARNINGS;
        IsFocusable = false;
        PRAGMA_ENABLE_DEPRECATION_WARNINGS;

        OnClicked.AddDynamic(this, &ThisClass::HandleOnClicked);
        OnHovered.AddDynamic(this, &ThisClass::HandleOnHovered);
        OnUnhovered.AddDynamic(this, &ThisClass::HandleOnUnhovered);
    }

private:
    UFUNCTION()
    void HandleOnClicked() { OnTabButtonClicked.ExecuteIfBound(TabTag); }

    UFUNCTION()
    void HandleOnHovered() { OnTabButtonHovered.ExecuteIfBound(TabTag); }

    UFUNCTION()
    void HandleOnUnhovered() { OnTabButtonUnhovered.ExecuteIfBound(TabTag); }

protected:
    UPROPERTY(EditAnywhere, Category = "Tab")
    FGameplayTag TabTag;
};