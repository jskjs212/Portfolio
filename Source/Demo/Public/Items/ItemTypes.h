// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTypes.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct DEMO_API FItemSlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Item")
    FDataTableRowHandle ItemID;

    UPROPERTY(EditAnywhere, Category = "Item")
    uint32 Quantity{1};

    // Slot is being equipped or consumed.
    UPROPERTY(VisibleAnywhere)
    bool bIsUsing{false};
};

USTRUCT(BlueprintType)
struct DEMO_API FItemArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<FItemSlot> Items;
};