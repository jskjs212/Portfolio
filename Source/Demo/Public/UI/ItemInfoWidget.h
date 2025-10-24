// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoTypes/ItemTypes.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ItemInfoWidget.generated.h"

struct FItemDataBase;
class UDynamicEntryBox;
class UImage;
class UTextBlock;
class UTexture2D;

/**
 *
 */
UCLASS()
class DEMO_API UItemInfoWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    UItemInfoWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;

public:
    // @return ItemData if valid.
    const FItemDataBase* UpdateItemInfo(const FItemSlot& InSlot);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CategoryImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDynamicEntryBox> StatModifierEntryBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DescriptionText;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    FDataTableRowHandle CachedItemRowHandle;

    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TMap<FGameplayTag, TObjectPtr<UTexture2D>> CategoryImages;
};