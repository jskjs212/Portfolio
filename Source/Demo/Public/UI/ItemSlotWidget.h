// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoTypes/ItemTypes.h"
#include "Engine/DataTable.h"
#include "ItemSlotWidget.generated.h"

class UBorder;
class UDraggedItemSlotWidget;
class UImage;
class USizeBox;
class USoundBase;
class UTextBlock;
class UTexture2D;

DECLARE_DELEGATE_TwoParams(FOnItemSlotClicked, const FItemSlot& /* InSlot */, int32 /* InIndex */);
DECLARE_DELEGATE_OneParam(FOnItemSlotHovered, const FItemSlot& /* InSlot */);
DECLARE_DELEGATE_FourParams(FOnItemSlotDropped, const FItemSlot& /* SrcSlot */, int32 /* SrcIndex */, const FItemSlot& /* DstSlot */, int32 /* DstIndex */);

/**
 * Item slot widget that can be used in inventory, equipment, etc.
 * Allows drag & drop, click, hover events.
 */
UCLASS()
class DEMO_API UItemSlotWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnItemSlotClicked OnRightClicked;
    FOnItemSlotClicked OnLeftDoubleClicked;
    FOnItemSlotHovered OnHovered;
    FSimpleDelegate OnUnhovered;
    FOnItemSlotDropped OnDropped;

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    // LMB: Drag, RMB: Event (context menu)
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // Event (hover) + change image, color + sound
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // Event (unhover) + change image, color
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    // Event (use, unequip)
    virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

    // Event (swap, move, merge)
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    void UpdateVisuals();

    FORCEINLINE void SetIndex(int32 InIndex) { Index = InIndex; }

    FORCEINLINE void SetSourceTag(FGameplayTag InSourceTag) { SourceTag = InSourceTag; }

    FORCEINLINE void SetItemSlot(const FItemSlot& InItemSlot) { ItemSlot = InItemSlot; }

private:
    void HandleHovered();

    void HandleUnhovered();

    FORCEINLINE bool CanDragDrop() const { return ItemSlot.IsValid() && !ItemSlot.bIsLocked; }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> ItemBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> HoveredBorderTriangleImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USizeBox> QuantitySizeBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> QuantityText;

    ////////////////////////////////////////////////////////
    //        Variables - Item
    ////////////////////////////////////////////////////////
private:
    // The owner allocates an index, and uses it to identify the slot.
    // i.e. Index in the inventory array.
    int32 Index{-1};

    // Source where the item slot belongs to.
    // i.e. UI_PlayerMenu_Inventory
    FGameplayTag SourceTag;

    FItemSlot ItemSlot;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    FDataTableRowHandle CurrentItemRowHandle;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FLinearColor NormalBorderColor{FLinearColor::White};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FLinearColor HoveredBorderColor{FLinearColor{1.f, 1.f, 1.f, 0.3f}};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<UTexture2D> NormalBorderImage;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<UTexture2D> HoveredBorderImage;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    TSubclassOf<UDraggedItemSlotWidget> DraggedItemSlotWidgetBPClass;

    // @TODO - Audio
    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<USoundBase> HoveredSound;
};