// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "Items/ItemTypes.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    OwnedItems.Add(DemoGameplayTags::Item_Weapon, FItemArray{});
    OwnedItems.Add(DemoGameplayTags::Item_Armor, FItemArray{});
    OwnedItems.Add(DemoGameplayTags::Item_Consumable, FItemArray{});
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // TEST:
    if (OwnedItems.Num())
    {
        int32 Temp = OwnedItems.begin()->Value.Items.Add(FItemSlot{});
    }
}