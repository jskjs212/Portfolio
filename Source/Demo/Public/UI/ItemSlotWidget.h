// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Items/ItemTypes.h"
#include "ItemSlotWidget.generated.h"

class UBorder;
class UImage;
class USizeBox;
class UTextBlock;
class UTexture2D;

/**
 *
 */
UCLASS()
class DEMO_API UItemSlotWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

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
protected:
    UPROPERTY(VisibleAnywhere, Category = "Item")
    int32 Index{-1};

    UPROPERTY(VisibleAnywhere, Category = "Item")
    FItemSlot ItemSlot;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Appearance|Item")
    FLinearColor NormalBorderColor{FLinearColor::White};

    UPROPERTY(EditDefaultsOnly, Category = "Appearance|Item")
    FLinearColor HoveredBorderColor{FLinearColor{1.f, 1.f, 1.f, 0.3f}};

    UPROPERTY(EditDefaultsOnly, Category = "Appearance|Item")
    TObjectPtr<UTexture2D> NormalBorderImage;

    UPROPERTY(EditDefaultsOnly, Category = "Appearance|Item")
    TObjectPtr<UTexture2D> HoveredBorderImage;

private:
    FDataTableRowHandle CurrentRowHandle;
};