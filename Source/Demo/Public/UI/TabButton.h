// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DemoTypes/LogCategories.h"
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

        OnClicked.AddDynamic(this, &ThisClass::HandleOnClicked);
        OnHovered.AddDynamic(this, &ThisClass::HandleOnHovered);
        OnUnhovered.AddDynamic(this, &ThisClass::HandleOnUnhovered);
    }

    FORCEINLINE FGameplayTag GetTypeTag() const { return TypeTag; }

    FORCEINLINE void SetTypeTag(FGameplayTag InTag) { TypeTag = InTag; }

private:
    UFUNCTION()
    void HandleOnClicked() { OnTabButtonClicked.ExecuteIfBound(TypeTag); }

    UFUNCTION()
    void HandleOnHovered() { OnTabButtonHovered.ExecuteIfBound(TypeTag); }

    UFUNCTION()
    void HandleOnUnhovered() { OnTabButtonUnhovered.ExecuteIfBound(TypeTag); }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    FGameplayTag TypeTag;
};