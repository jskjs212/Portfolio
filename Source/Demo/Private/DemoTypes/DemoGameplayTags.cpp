// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoTypes/DemoGameplayTags.h"

namespace DemoGameplayTags
{
// Item.Weapon.*
UE_DEFINE_GAMEPLAY_TAG(Item_Weapon, "Item.Weapon");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Weapon_NoWeapon, "Item.Weapon.NoWeapon", "NoWeapon tag is for ActionInfoConfig without weapons.")
UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_Melee_OneHanded, "Item.Weapon.Melee.OneHanded");
UE_DEFINE_GAMEPLAY_TAG(Item_Weapon_Melee_TwoHanded, "Item.Weapon.Melee.TwoHanded");

// Item.Armor.*
UE_DEFINE_GAMEPLAY_TAG(Item_Armor, "Item.Armor");
UE_DEFINE_GAMEPLAY_TAG(Item_Armor_Shield, "Item.Armor.Shield");

// Item.Consumable.*
UE_DEFINE_GAMEPLAY_TAG(Item_Consumable, "Item.Consumable");
UE_DEFINE_GAMEPLAY_TAG(Item_Consumable_Food, "Item.Consumable.Food");

// Movement.SpeedMode.*
UE_DEFINE_GAMEPLAY_TAG(Movement_SpeedMode_Walk, "Movement.SpeedMode.Walk");
UE_DEFINE_GAMEPLAY_TAG(Movement_SpeedMode_Jog, "Movement.SpeedMode.Jog");
UE_DEFINE_GAMEPLAY_TAG(Movement_SpeedMode_Sprint, "Movement.SpeedMode.Sprint");

// Character states: State.[state]
UE_DEFINE_GAMEPLAY_TAG(State, "State");
UE_DEFINE_GAMEPLAY_TAG(State_General, "State.General");
UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
UE_DEFINE_GAMEPLAY_TAG(State_Disabled, "State.Disabled");
UE_DEFINE_GAMEPLAY_TAG(State_Jump, "State.Jump");
UE_DEFINE_GAMEPLAY_TAG(State_ToggleCombat, "State.ToggleCombat");
UE_DEFINE_GAMEPLAY_TAG(State_Attack, "State.Attack");
UE_DEFINE_GAMEPLAY_TAG(State_Dodge, "State.Dodge");
UE_DEFINE_GAMEPLAY_TAG(State_Block, "State.Block");

// Character actions: State.[state].[action]
UE_DEFINE_GAMEPLAY_TAG(State_ToggleCombat_Enter, "State.ToggleCombat.Enter");
UE_DEFINE_GAMEPLAY_TAG(State_ToggleCombat_Exit, "State.ToggleCombat.Exit");
UE_DEFINE_GAMEPLAY_TAG(State_Attack_Light, "State.Attack.Light");
UE_DEFINE_GAMEPLAY_TAG(State_Attack_Heavy, "State.Attack.Heavy");
UE_DEFINE_GAMEPLAY_TAG(State_Attack_Charged, "State.Attack.Charged");

// StateTree.*
UE_DEFINE_GAMEPLAY_TAG(StateTree_Tree_Main, "StateTree.Tree.Main");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_TargetLost, "StateTree.Event.TargetLost");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_TargetSensed, "StateTree.Event.TargetSensed");

// UI.PlayerMenu.*
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Stats, "UI.PlayerMenu.Stats");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Inventory, "UI.PlayerMenu.Inventory");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Equipment, "UI.PlayerMenu.Equipment");

// UI.Action.Item.*
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Use, "UI.Action.Item.Use");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Drop, "UI.Action.Item.Drop");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Unequip, "UI.Action.Item.Unequip");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Cancel, "UI.Action.Item.Cancel");
}