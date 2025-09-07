// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatsPageWidget.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UStatsPageWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

    // TEST:
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentHealthText;
};