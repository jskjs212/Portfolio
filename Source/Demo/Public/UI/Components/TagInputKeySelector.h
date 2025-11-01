// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputKeySelector.h"
#include "DemoTypes/LogCategories.h"
#include "GameplayTagContainer.h"
#include "TagInputKeySelector.generated.h"

DECLARE_DELEGATE_TwoParams(FOnTagInputKeySelectorKeySelected, FGameplayTag /* InTag */, FInputChord /* InSelectedKey */);
DECLARE_DELEGATE_OneParam(FOnTagInputKeySelectorIsSelectingKeyChanged, FGameplayTag /* InTag */);

/**
 * Input key selector that has a tag.
 */
UCLASS()
class DEMO_API UTagInputKeySelector : public UInputKeySelector
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnTagInputKeySelectorKeySelected OnTagInputKeySelectorKeySelected;
    FOnTagInputKeySelectorIsSelectingKeyChanged OnTagInputKeySelectorIsSelectingKeyChanged;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void OnWidgetRebuilt() override
    {
        Super::OnWidgetRebuilt();
        DemoLOG_CF(LogTEST, Display, TEXT("Name: %s"), *GetName());
        OnKeySelected.AddDynamic(this, &ThisClass::HandleTagInputKeySelectorKeySelected);
        OnIsSelectingKeyChanged.AddDynamic(this, &ThisClass::HandleTagInputKeySelectorIsSelectingKeyChanged);
    }

public:
    FORCEINLINE FGameplayTag GetTypeTag() const { return TypeTag; }

    FORCEINLINE void SetTypeTag(FGameplayTag InTag) { TypeTag = InTag; }

private:
    UFUNCTION()
    void HandleTagInputKeySelectorKeySelected(FInputChord InSelectedKey)
    {
        OnTagInputKeySelectorKeySelected.ExecuteIfBound(TypeTag, InSelectedKey);
    }

    UFUNCTION()
    void HandleTagInputKeySelectorIsSelectingKeyChanged()
    {
        OnTagInputKeySelectorIsSelectingKeyChanged.ExecuteIfBound(TypeTag);
    }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    FGameplayTag TypeTag;
};
