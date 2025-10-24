// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoTypes/DemoTypes.h"
#include "CursorWidget.generated.h"

class UImage;
class UTexture2D;

/**
 *
 */
UCLASS()
class DEMO_API UCursorWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

public:
    void UpdateCursorVisuals(ECursorState NewCursorState);

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CursorImage;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    TMap<ECursorState, TObjectPtr<UTexture2D>> CursorImageMap;
};