// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DemoTypes/LogCategories.h"
#include "GameplayTagContainer.h"
#include "TagButton.generated.h"

DECLARE_DELEGATE_OneParam(FOnTagButtonEvent, FGameplayTag /* InTag */);

/**
 * Button that has a tag.
 * When button events are triggered, the owner of this button can handle them with the tag in a generic way.
 */
UCLASS()
class DEMO_API UTagButton : public UButton
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnTagButtonEvent OnTagButtonClicked;
    FOnTagButtonEvent OnTagButtonHovered;
    FOnTagButtonEvent OnTagButtonUnhovered;

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
    void HandleOnClicked() { OnTagButtonClicked.ExecuteIfBound(TypeTag); }

    UFUNCTION()
    void HandleOnHovered() { OnTagButtonHovered.ExecuteIfBound(TypeTag); }

    UFUNCTION()
    void HandleOnUnhovered() { OnTagButtonUnhovered.ExecuteIfBound(TypeTag); }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    FGameplayTag TypeTag;
};