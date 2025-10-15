// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDHelpWidget.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UHUDHelpWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

public:
    void ToggleHelpText();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HelpText;

private:
    bool bIsHelpTextVisible{false};
};