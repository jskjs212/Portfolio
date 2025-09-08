// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InventoryPageWidget.h"
#include "Components/Image.h"
#include "Components/WrapBox.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "UI/TabButton.h"

void UInventoryPageWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(WeaponTabButton && WeaponTabImage && WeaponWrapBox &&
        ArmorTabButton && ArmorTabImage && ArmorWrapBox &&
        ConsumableTabButton && ConsumableTabImage && ConsumableWrapBox,
        TEXT("Failed to bind widgets."));

    bUseTabButtonImages = true;

    // Setup tab entries
    // Should sync with DemoGameplayTags::ItemCategories.
    TabEntries.Add(FTabEntry{DemoGameplayTags::Item_Weapon, WeaponTabButton, WeaponTabImage, WeaponWrapBox});
    TabEntries.Add(FTabEntry{DemoGameplayTags::Item_Armor, ArmorTabButton, ArmorTabImage, ArmorWrapBox});
    TabEntries.Add(FTabEntry{DemoGameplayTags::Item_Consumable, ConsumableTabButton, ConsumableTabImage, ConsumableWrapBox});

    InitMenu();
}

FReply UInventoryPageWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    static const TMap<FKey, int32> KeyToIndexMap{
        {EKeys::One, 0},
        {EKeys::Two, 1},
        {EKeys::Three, 2},
        {EKeys::Four, 3},
        {EKeys::Five, 4},
        {EKeys::Six, 5},
        {EKeys::Seven, 6},
        {EKeys::Eight, 7},
        {EKeys::Nine, 8}
    };

    // Key(1~9) -> select tab
    const int32* IndexPtr = KeyToIndexMap.Find(InKeyEvent.GetKey());
    if (IndexPtr)
    {
        if (*IndexPtr < TabEntries.Num())
        {
            SelectTab(*IndexPtr);
            return FReply::Handled();
        }
    }

    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}