// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "EquipmentPageWidget.generated.h"

class UContextMenuWidget;
class UItemActionDispatcher;
class UItemInfoWidget;
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

    FGameplayTag GetEquipmentTypeAtIndex(int32 InIndex) const
    {
        return EquipmentSlots.IsValidIndex(InIndex) ? EquipmentSlots[InIndex].EquipmentType : FGameplayTag::EmptyTag;
    }

private:
    void InitEquipmentSlots();

    void InitContextMenu();

    void BindToEquipmentUpdates();

    UItemActionDispatcher* GetItemActionDispatcher() const;

    // Only for RMB context menu = valid index.
    FORCEINLINE void SetContextMenuData(int32 InIndex)
    {
        ContextMenuEquipmentType = EquipmentSlots[InIndex].EquipmentType;
    }

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    void HandleContextMenuButtonClicked(FGameplayTag InTag);

    void HandleItemSlotRightClicked(const FItemSlot& InSlot, int32 InIndex);

    void HandleItemSlotLeftDoubleClicked(const FItemSlot& InSlot, int32 InIndex);

    void ShowItemInfo(const FItemSlot& InSlot);

    void HideItemInfo();

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
protected:
    /* Equipment slots */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemSlotWidget> WeaponSlot;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemSlotWidget> ShieldSlot;

    /* Else */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemInfoWidget> ItemInfoWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UContextMenuWidget> ContextMenuWidget;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    FGameplayTag ContextMenuEquipmentType;

    TArray<FEquipmentSlotData> EquipmentSlots;
};