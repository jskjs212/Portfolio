// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "GameplayTagContainer.h"
#include "TabButton.generated.h"

DECLARE_DELEGATE_OneParam(FOnTabButtonEvent, FGameplayTag /* InTag */);

/**
 * Tab button that has a tab type.
 * When button events are triggered, the owner of this button can handle them with the tab type in generic way.
 */
UCLASS()
class DEMO_API UTabButton : public UButton
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnTabButtonEvent OnTabButtonClicked;
    FOnTabButtonEvent OnTabButtonHovered;
    FOnTabButtonEvent OnTabButtonUnhovered;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    virtual void SynchronizeProperties() override
    {
        Super::SynchronizeProperties();

        if (!TabTag.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("TabTag is not set in %s"), *GetName());
        }

        OnClicked.AddDynamic(this, &ThisClass::HandleOnClicked);
        OnHovered.AddDynamic(this, &ThisClass::HandleOnHovered);
        OnUnhovered.AddDynamic(this, &ThisClass::HandleOnUnhovered);
    }

    FORCEINLINE FGameplayTag GetTabTag() const { return TabTag; }

    FORCEINLINE void SetTabTag(FGameplayTag InTag) { TabTag = InTag; }

private:
    UFUNCTION()
    void HandleOnClicked() { OnTabButtonClicked.ExecuteIfBound(TabTag); }

    UFUNCTION()
    void HandleOnHovered() { OnTabButtonHovered.ExecuteIfBound(TabTag); }

    UFUNCTION()
    void HandleOnUnhovered() { OnTabButtonUnhovered.ExecuteIfBound(TabTag); }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    FGameplayTag TabTag;
};