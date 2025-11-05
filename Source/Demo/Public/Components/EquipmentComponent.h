// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "EquipmentComponent.generated.h"

struct FEquipmentSaveData;
struct FWeaponData;
class AItem;
class UDemoAudioSubsystem;
class UInventoryComponent;
class UStateManagerComponent;

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

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, FGameplayTag /* EquipmentType */, const FItemSlot& /* InSlot */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, const FWeaponData* /* WeaponData */);

/**
 * Equipment component attached to ACharacter.
 * Fixed EquipmentTypes and SocketNames:
 *   Item.Weapon = MeleeHandSocket
 *   Item.Armor.Shield = ShieldHandSocket
 * @Dependency - UnequipItem() needs InventoryComponent.
 * @Dependency - Equip & unequip functions query StateManagerComponent if it exists.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    // @param Tag is the EquipmentType.
    // @param InSlot is the equipped item slot.
    FOnEquipmentChanged OnEquipped;

    // @param Tag is the EquipmentType.
    // @param InSlot is the unequipped item slot.
    FOnEquipmentChanged OnUnequipped;

    // @param nullptr if unequipped.
    FOnWeaponChanged OnWeaponChanged;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
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

    void PopulateSaveData(FEquipmentSaveData& OutData) const;

    void LoadFromSaveData(const FEquipmentSaveData& InData);

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

    bool UnequipItem_AddToInventory(const FItemSlot& InSlot);

    void UnequipItem_PostProcess(FGameplayTag EquipmentType, const FItemSlot& UnequippedSlot);

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    bool HandleUnequipItemActionRequested(FGameplayTag InActionTag, FGameplayTag EquipmentType);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    // @return nullptr if not found
    AItem* GetEquippedItem(FGameplayTag EquipmentType) const;

    const TMap<FGameplayTag, TObjectPtr<AItem>>& GetAllEquippedItems() const { return EquippedItems; }

private:
    // Get InventoryComponent from the owner's controller
    UInventoryComponent* GetInventoryComponent();

    const UStateManagerComponent* GetStateManager();

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    // nullptr = not equipped
    UPROPERTY(VisibleAnywhere, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, TObjectPtr<AItem>> EquippedItems;

    UPROPERTY(EditDefaultsOnly, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, FName> EquipDefaultSocketNames;

    // Cache ItemType of weapon
    FGameplayTag CurrentWeaponType;

    TWeakObjectPtr<UInventoryComponent> CachedInventoryComponent;
    TWeakObjectPtr<const UStateManagerComponent> CachedStateManager;
    TObjectPtr<UDemoAudioSubsystem> AudioSubsystem;
};