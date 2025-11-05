// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "Items/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "DemoSaveGame.generated.h"

USTRUCT()
struct FPlayerCharacterSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    FTransform Transform;
};

USTRUCT()
struct FInventorySaveData
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<FGameplayTag, FItemArray> OwnedItems;
};

// Equip again with the ItemSlot data when loading.
USTRUCT()
struct FEquipmentSaveData
{
    GENERATED_BODY()

    // FItemSlot{} if unequipped.
    UPROPERTY()
    TMap<FGameplayTag, FItemSlot> EquippedItemSlots;
};

/**
 * Save game class for Demo project.
 * DemoGameInstance manages saving and loading.
 *
 * Consider adding: Quest progress, World state, etc.
 * @TODO - Multiple save slots, version control.
 */
UCLASS()
class DEMO_API UDemoSaveGame : public USaveGame
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Statics
    //////////////////////////////////////////////////////// 
public:
    // Increment when save data structure changes.
    static inline const float CurrentSaveGameVersion{1.f};

    static inline const int32 DefaultUserIndex{0};
    static inline const FString DefaultSaveSlot{TEXT("DemoSaveSlot")};

    ////////////////////////////////////////////////////////
    //        Save data
    //////////////////////////////////////////////////////// 
public:
    // Discard saves from incompatible versions.
    // @TODO - Handle with old versions.
    UPROPERTY()
    float SavedSaveGameVersion{0.f};

    UPROPERTY()
    FPlayerCharacterSaveData PlayerCharacterData;

    UPROPERTY()
    FInventorySaveData InventoryData;

    UPROPERTY()
    FEquipmentSaveData EquipmentData;
};