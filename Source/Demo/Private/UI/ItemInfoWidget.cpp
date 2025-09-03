// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ItemInfoWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DemoTypes/TableRowBases.h"

UItemInfoWidget::UItemInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // GameplayTags are initialized(?) before subsystem's init.
    for (const FGameplayTag& ItemCategory : DemoItemTypes::ItemCategories)
    {
        CategoryImages.Add(ItemCategory, nullptr);
    }
}

void UItemInfoWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // Validation
    if (!CategoryImage || !NameText || !DescriptionText)
    {
        UE_LOG(LogTemp, Error, TEXT("UItemInfoWidget - Failed to bind widgets."));
        return;
    }

    checkf(CategoryImages.Num() == DemoItemTypes::ItemCategories.Num(), TEXT("CategoryImages should have all ItemCategories."));
    for (const TPair<FGameplayTag, TObjectPtr<UTexture2D>>& Pair : CategoryImages)
    {
        if (!Pair.Value)
        {
            UE_LOG(LogTemp, Error, TEXT("UItemInfoWidget - CategoryImages has no image for %s."), *Pair.Key.ToString());
        }
    }
}

void UItemInfoWidget::UpdateItemInfo(const FItemSlot& InSlot)
{
    const FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UItemInfoWidget::UpdateItemInfo()"));
    if (!ItemData)
    {
        return;
    }

    // Update
    FGameplayTag ItemCategory = DemoItemTypes::GetItemCategory(ItemData->ItemType);
    CategoryImage->SetBrushFromTexture(CategoryImages[ItemCategory]);
    NameText->SetText(FText::FromName(ItemData->Name));
    DescriptionText->SetText(ItemData->Description);
}