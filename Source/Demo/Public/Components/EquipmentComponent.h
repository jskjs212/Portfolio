// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Items/ItemTypes.h"
#include "EquipmentComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEquipment, Log, All);

class AItem;

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
    // check: What if Quantity > 1?
    // misc: Return EEquipResult if needed.
    // @return true if successfully equipped
    bool EquipItem(const FItemSlot& InSlot);

    // @return true if successfully unequipped
    bool UnequipItem(FGameplayTag EquipmentType);

private:
    // @return nullptr if failed
    AItem* SpawnItemToEquip(APawn* OwnerPawn, const FItemSlot& InSlot) const;

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
private:
    // nullptr = not equipped
    UPROPERTY(VisibleAnywhere, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, TObjectPtr<AItem>> EquippedItems;

    UPROPERTY(EditDefaultsOnly, Category = "Item", meta = (Categories = "Item"))
    TMap<FGameplayTag, FName> EquipDefaultSocketNames;
};