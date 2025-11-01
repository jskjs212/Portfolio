// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "DemoTypes/LogCategories.h"
#include "GameplayTagContainer.h"
#include "TagSlider.generated.h"

DECLARE_DELEGATE_OneParam(FOnTagSliderMouseCaptureEvent, FGameplayTag /* InTag */);
DECLARE_DELEGATE_OneParam(FOnTagSliderControllerCaptureEvent, FGameplayTag /* InTag */);
DECLARE_DELEGATE_TwoParams(FOnTagSliderValueChanged, FGameplayTag /* InTag */, float /* InValue */);

/**
 * Slider that has a tag.
 * When slider events are triggered, the owner of this slider can handle them with the tag in a generic way.
 */
UCLASS()
class DEMO_API UTagSlider : public USlider
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnTagSliderMouseCaptureEvent OnTagSliderMouseCaptureEnd;
    FOnTagSliderControllerCaptureEvent OnTagSliderControllerCaptureEnd;
    FOnTagSliderValueChanged OnTagSliderValueChanged;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void OnWidgetRebuilt() override
    {
        Super::OnWidgetRebuilt();
        DemoLOG_CF(LogTEST, Display, TEXT("Name: %s"), *GetName());
        OnMouseCaptureEnd.AddDynamic(this, &ThisClass::HandleTagSliderMouseCaptureEnd);
        OnControllerCaptureEnd.AddDynamic(this, &ThisClass::HandleTagSliderControllerCaptureEnd);
        OnValueChanged.AddDynamic(this, &ThisClass::HandleTagSliderValueChanged);
    }

public:
    FORCEINLINE FGameplayTag GetTypeTag() const { return TypeTag; }

    FORCEINLINE void SetTypeTag(FGameplayTag InTag) { TypeTag = InTag; }

private:
    UFUNCTION()
    void HandleTagSliderMouseCaptureEnd() { OnTagSliderMouseCaptureEnd.ExecuteIfBound(TypeTag); }

    UFUNCTION()
    void HandleTagSliderControllerCaptureEnd() { OnTagSliderControllerCaptureEnd.ExecuteIfBound(TypeTag); }

    UFUNCTION()
    void HandleTagSliderValueChanged(float InValue) { OnTagSliderValueChanged.ExecuteIfBound(TypeTag, InValue); }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    FGameplayTag TypeTag;
};