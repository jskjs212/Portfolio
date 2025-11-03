// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ItemInfoWidget.h"
#include "Components/DynamicEntryBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "DemoTypes/LogCategories.h"
#include "Items/ItemRowBases.h"
#include "UI/StatModifierWidget.h"

UItemInfoWidget::UItemInfoWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // GameplayTags are initialized(?) before subsystem's init.
    for (const FGameplayTag& ItemCategory : Demo::Item::GetItemCategories())
    {
        CategoryImages.Add(ItemCategory, nullptr);
    }
}

void UItemInfoWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(CategoryImage && NameText && StatModifierEntryBox && DescriptionText,
        TEXT("Failed to bind widgets."));
    checkf(StatModifierEntryBox->GetEntryWidgetClass()->IsChildOf(UStatModifierWidget::StaticClass()),
        TEXT("Invalid EntryWidgetClass for StatModifierEntryBox."));

    if (CategoryImages.Num() != Demo::Item::GetItemCategories().Num())
    {
        DemoLOG_CF(LogUI, Error, TEXT("CategoryImages should have all ItemCategories."));
    }

    for (const auto& [ItemCategory, CategoryImageRef] : CategoryImages)
    {
        if (!CategoryImageRef)
        {
            DemoLOG_CF(LogUI, Error, TEXT("CategoryImages has no image for %s."), *ItemCategory.ToString());
        }
    }
}

const FItemDataBase* UItemInfoWidget::UpdateItemInfo(const FItemSlot& InSlot)
{
    const FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UItemInfoWidget::UpdateItemInfo()"));
    if (!ItemData)
    {
        return nullptr;
    }

    if (CachedItemRowHandle == InSlot.RowHandle)
    {
        // No change
        return ItemData;
    }
    CachedItemRowHandle = InSlot.RowHandle;

    // Update
    FGameplayTag ItemCategory = Demo::Item::GetItemCategory(ItemData->ItemType);
    CategoryImage->SetBrushFromTexture(CategoryImages[ItemCategory]);
    StatModifierEntryBox->Reset();
    NameText->SetText(FText::FromName(ItemData->Name));
    DescriptionText->SetText(ItemData->Description);

    // Stat modifier entries
    bool bIsEquipment = Demo::Item::GetEquipmentType(ItemData->ItemType).IsValid();
    if (bIsEquipment)
    {
        if (const FEquipmentData* EquipmentData = InSlot.RowHandle.GetRow<FEquipmentData>(TEXT("UItemInfoWidget::UpdateItemInfo()")))
        {
            for (const auto& [StatTag, StatModifier] : EquipmentData->StatModifiers)
            {
                if (UStatModifierWidget* StatModifierWidget = StatModifierEntryBox->CreateEntry<UStatModifierWidget>())
                {
                    StatModifierWidget->Update(StatTag, StatModifier);
                }
                else
                {
                    DemoLOG_CF(LogUI, Error, TEXT("Failed to create StatModifierWidget entry."));
                }
            }
        }
    }
    return ItemData;
}