// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTypes.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FItemSlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Item")
    FDataTableRowHandle ItemID;

    UPROPERTY(EditAnywhere, Category = "Item")
    int32 Quantity{0};

    // We need only quantity at the moment.
    // Scalability example: FWeaponInstanceData (Durability, Enchant, MasteryLevel, Attachment, etc.)
    //UPROPERTY(VisibleAnywhere)
    //TObjectPtr<FItemInstanceData> InstanceData;

    FORCEINLINE bool IsValid() const { return !ItemID.IsNull() && Quantity > 0; }
};

USTRUCT(BlueprintType)
struct FItemArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<FItemSlot> ItemArray;
};