// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractWidget.generated.h"

struct FItemSlot;
class UItemInfoWidget;

/**
 *
 */
UCLASS()
class DEMO_API UInteractWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void NativeOnInitialized() override;

    void UpdateUI(const FItemSlot& InSlot);

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemInfoWidget> ItemInfo;
};