// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionKeyWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

/**
 * Display the input key for an action.
 * Set key text and image manually for now.
 * Maybe later, find key bindings and set text by action tag.
 */
UCLASS()
class DEMO_API UActionKeyWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    virtual void NativePreConstruct() override;

public:
    void SetKeyText(const FText& InText);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> KeyTextBlock;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> BackgroundImage;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "ActionKey")
    FText KeyText;

    UPROPERTY(EditAnywhere, Category = "ActionKey")
    TObjectPtr<UTexture2D> BackgroundTexture;
};