// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "EquipmentComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEquipment, Log, All);

class AItem;
class USoundBase;

/**
 * Used to return validated data from internal functions.
 */
USTRUCT()
struct FEquipmentValidationData
{
    GENERATED_BODY()

    bool bIsValid{false};

    FGameplayTag ItemType;

    FGameplayTag EquipmentType;

    TObjectPtr<AItem>* EquippedItemPtr{nullptr};
};

DECLARE_DELEGATE_OneParam(FOnEquipmentChanged, FGameplayTag /* ItemType */);

/**
 * Equipment
 * Fixed EquipmentTypes and SocketNames:
 *   Item.Weapon = MeleeHandSocket
 *   Item.Armor.Shield = ShieldHandSocket
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    // @param ItemType is the type from ItemData.
    FOnEquipmentChanged OnWeaponChanged;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UEquipmentComponent();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Equipment functions
    ////////////////////////////////////////////////////////
public:
    // InSlot's quantity is not changed.
    // @misc - Return EEquipResult if needed.
    // @return true if successfully equipped
    bool EquipItem(const FItemSlot& InSlot);

    // @return true if successfully unequipped
    bool UnequipItem(FGameplayTag EquipmentType);

    void DestroyAllEquippedItems();

private:
    // @return .bIsValid is true if valid. If false, data is not valid.
    FEquipmentValidationData EquipItem_Validate(const FItemSlot& InSlot);

    // @return nullptr if failed
    AItem* EquipItem_SpawnItem(const FItemSlot& InSlot) const;

    // @return true if successfully attached
    bool AttachActor(AActor* ActorToAttach, FName SocketName) const;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    // @return nullptr if not found
    AItem* GetEquippedItem(FGameplayTag EquipmentType) const;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
public:
    // @TODO - Audio
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    TObjectPtr<USoundBase> EquipSound;

private:
    // nullptr = not equipped
    UPROPERTY(VisibleAnywhere, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, TObjectPtr<AItem>> EquippedItems;

    UPROPERTY(EditDefaultsOnly, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, FName> EquipDefaultSocketNames;
};