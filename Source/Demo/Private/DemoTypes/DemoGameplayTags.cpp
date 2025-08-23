// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoTypes/DemoGameplayTags.h"

namespace DemoGameplayTags
{
// Movement.SpeedMode.*
UE_DEFINE_GAMEPLAY_TAG(Movement_SpeedMode_Walk, "Movement.SpeedMode.Walk");
UE_DEFINE_GAMEPLAY_TAG(Movement_SpeedMode_Jog, "Movement.SpeedMode.Jog");
UE_DEFINE_GAMEPLAY_TAG(Movement_SpeedMode_Sprint, "Movement.SpeedMode.Sprint");

// Item.Weapon.*
UE_DEFINE_GAMEPLAY_TAG(Item_Weapon, "Item.Weapon");
UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_Melee_OneHanded, "Item.Weapon.Melee.OneHanded");
UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_Melee_TwoHanded, "Item.Weapon.Melee.TwoHanded");

// Item.Armor.*
UE_DEFINE_GAMEPLAY_TAG(Item_Armor, "Item.Armor");
UE_DEFINE_GAMEPLAY_TAG(Item_Armor_Shield, "Item.Armor.Shield");

// Item.Consumable.*
UE_DEFINE_GAMEPLAY_TAG(Item_Consumable, "Item.Consumable");
UE_DEFINE_GAMEPLAY_TAG(Item_Consumable_Food, "Item.Consumable.Food");
}