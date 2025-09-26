// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
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

    FORCEINLINE FGameplayTag GetSourceTag() const { return SourceTag; }

    FORCEINLINE const FItemSlot& GetItemSlot() const { return ItemSlot; }

    FORCEINLINE void Setup(int32 InIndex, FGameplayTag InSourceTag, const FItemSlot& InSlot)
    {
        Index = InIndex;
        SourceTag = InSourceTag;
        ItemSlot = InSlot;
    }

private:
    int32 Index{-1};

    FGameplayTag SourceTag;

    FItemSlot ItemSlot;
};