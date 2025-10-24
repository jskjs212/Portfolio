// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "StatModifierWidget.generated.h"

struct FStatModifier;
class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UStatModifierWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

public:
    void Update(FGameplayTag StatTag, const FStatModifier& InStatModifier);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> StatNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> StatValueText;
};