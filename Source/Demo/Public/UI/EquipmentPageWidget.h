// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "EquipmentPageWidget.generated.h"

class UContextMenuWidget;
class UItemActionDispatcher;
class UItemSlotWidget;

struct FEquipmentSlotData
{
    FGameplayTag EquipmentType;

    TObjectPtr<UItemSlotWidget> SlotWidget;
};

/**
 * @TODO
 * 0) WBP design
 * 1) Link with EquipmentComponent.
 * 2) Handle clicks
 * 3) Context menu
 * 4) Show item info on hover
 */
UCLASS()
class DEMO_API UEquipmentPageWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    // Update UI of equipment item slots from owner pawn's EquipmentComponent
    void UpdateEquipmentSlotsUI(FGameplayTag EquipmentType);

private:
    void InitEquipmentSlots();

    void InitContextMenu();

    void BindToEquipmentUpdates();

    void HandleContextMenuButtonClicked(FGameplayTag InTag);

    UItemActionDispatcher* GetItemActionDispatcher() const;

    FORCEINLINE void SetActionRequest(const FItemSlot& InSlot, int32 InIndex)
    {
        ContextMenuItemActionRequest.Slot = InSlot;
        ContextMenuItemActionRequest.DesignatedIndex = InIndex;
        ContextMenuItemActionRequest.Quantity = 0;
    }

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    void HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemSlotWidget> WeaponSlot;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemSlotWidget> ShieldSlot;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UContextMenuWidget> ContextMenuWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    FItemActionRequest ContextMenuItemActionRequest;

    TArray<FEquipmentSlotData> EquipmentSlots;
};