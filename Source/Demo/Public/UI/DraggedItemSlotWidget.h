// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DraggedItemSlotWidget.generated.h"

class UImage;
class UTexture2D;

/**
 * Dragged item slot widget shown when dragging an item.
 */
UCLASS()
class DEMO_API UDraggedItemSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetItemIcon(UTexture2D* InIcon, float InOpacity);

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemIcon;
};