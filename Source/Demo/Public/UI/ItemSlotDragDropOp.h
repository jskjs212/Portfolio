// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Items/ItemTypes.h"
#include "ItemSlotDragDropOp.generated.h"

/**
 *
 */
UCLASS()
class DEMO_API UItemSlotDragDropOp : public UDragDropOperation
{
    GENERATED_BODY()

public:
    FORCEINLINE int32 GetIndex() const { return Index; }

    FORCEINLINE const FItemSlot& GetItemSlot() const { return ItemSlot; }

    FORCEINLINE void Setup(const FItemSlot& InSlot, int32 InIndex)
    {
        Index = InIndex;
        ItemSlot = InSlot;
    }

private:
    int32 Index{-1};

    FItemSlot ItemSlot;
};