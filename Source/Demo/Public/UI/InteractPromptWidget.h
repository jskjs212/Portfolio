// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractPromptWidget.generated.h"

class UActionKeyWidget;
class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UInteractPromptWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

public:
    // Currently, set only text.
    // Maybe later, set text by context (open, pickup, talk).
    void SetActionText(const FText& InText);

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UActionKeyWidget> ActionKeyWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ActionText;
};