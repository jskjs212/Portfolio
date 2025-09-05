// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionKeyWidget.generated.h"

class UTextBlock;

/**
 * Display the input key for an action.
 */
UCLASS()
class DEMO_API UActionKeyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Currently, set only text.
    // Maybe later, find key bindings and set text by action tag.
    void SetKeyText(const FText& InText);

    UPROPERTY(meta = (BindWidget))
    UTextBlock* KeyText;
};