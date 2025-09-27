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
struct FEquipmentValidationResult
{
    bool bIsValid{false};

    FGameplayTag ItemType;

    FGameplayTag EquipmentType;

    TObjectPtr<AItem>* EquippedItemPtr{nullptr};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, FGameplayTag /* ItemType */);

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
    // @param ItemType is EquipmentType
    FOnEquipmentChanged OnEquipmentChanged;

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

    // Unequip and keep in inventory.
    // @return true if successfully unequipped
    bool UnequipItem(FGameplayTag EquipmentType);

    // Unequip and drop.
    // @return true if successfully unequipped and dropped
    bool UnequipAndDropItem(FGameplayTag EquipmentType);

    void DestroyAllEquippedItems();

private:
    void InitEquipmentComponent();

    // Bind equipment functions to UI's item action dispatcher.
    void BindToItemActionDispatcher();

    // @return .bIsValid is true if valid. If false, data is not valid.
    FEquipmentValidationResult EquipItem_Validate(const FItemSlot& InSlot);

    // Check equipped items and unequip if necessary to equip the new item.
    bool EquipItem_HandleConflicts(const FEquipmentValidationResult& ValidationResult);

    // @return nullptr if failed
    AItem* EquipItem_SpawnItem(const FItemSlot& InSlot) const;

    // @return true if successfully attached
    bool AttachActor(AActor* ActorToAttach, FGameplayTag EquipmentType) const;

    void EquipItem_PostProcess(const FEquipmentValidationResult& ValidationResult);

    void UnequipItem_PostProcess(FGameplayTag EquipmentType);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    // @return nullptr if not found
    AItem* GetEquippedItem(FGameplayTag EquipmentType) const;

    const TMap<FGameplayTag, TObjectPtr<AItem>>& GetAllEquippedItems() const { return EquippedItems; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
public:
    // @TODO - Audio
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    TObjectPtr<USoundBase> EquipSound;

private:
    // Cache ItemType of weapon
    FGameplayTag CurrentWeaponType;

    // nullptr = not equipped
    UPROPERTY(VisibleAnywhere, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, TObjectPtr<AItem>> EquippedItems;

    UPROPERTY(EditDefaultsOnly, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, FName> EquipDefaultSocketNames;
};