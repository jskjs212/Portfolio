// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "DemoTypes/DemoGameplayTags.h"

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
    UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::BeginPlay - %d"), OwnedItems.Num());
    UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::BeginPlay - %d"), OwnedItems[DemoGameplayTags::Item_Weapon].Items.Num());
}