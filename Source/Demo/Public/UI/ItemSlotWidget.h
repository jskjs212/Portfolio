// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Items/ItemTypes.h"
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
 *
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

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    void UpdateItemSlot(const FItemSlot& InItemSlot, int32 InIndex)
    {
        ItemSlot = InItemSlot;
        Index = InIndex;
        UpdateVisuals();
    }

private:
    void UpdateVisuals();

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
    // Index in the inventory array
    UPROPERTY(VisibleAnywhere, Category = "Item")
    int32 Index{-1};

    UPROPERTY(VisibleAnywhere, Category = "Item")
    FItemSlot ItemSlot;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    FDataTableRowHandle CurrentItemRowHandle;

    UPROPERTY(EditDefaultsOnly, Category = "Item")
    FLinearColor NormalBorderColor{FLinearColor::White};

    UPROPERTY(EditDefaultsOnly, Category = "Item")
    FLinearColor HoveredBorderColor{FLinearColor{1.f, 1.f, 1.f, 0.3f}};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<UTexture2D> NormalBorderImage;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<UTexture2D> HoveredBorderImage;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TSubclassOf<UDraggedItemSlotWidget> DraggedItemSlotWidgetBPClass;

    // TODO: Audio
    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<USoundBase> HoveredSound;
};