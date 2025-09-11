// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/**
 *
 */
namespace DemoGameplayTags
{
// Movement.SpeedMode.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_SpeedMode_Walk);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_SpeedMode_Jog);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_SpeedMode_Sprint);

// Item.Weapon.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon_Melee_OneHanded);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon_Melee_TwoHanded);

// Item.Armor.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Armor);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Armor_Shield);

// Item.Consumable.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Consumable);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Consumable_Food);

// UI.PlayerMenu.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_PlayerMenu_Stats);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_PlayerMenu_Inventory);

// UI.Action.Item.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Use);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Drop);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Unequip);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Cancel);
}