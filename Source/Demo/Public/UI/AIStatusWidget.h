// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIStatusWidget.generated.h"

class UStatBarWidget;
class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UAIStatusWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

public:
    void BindToActor(AActor* InActor);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UStatBarWidget> HealthBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NameText;
};