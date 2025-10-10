// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/**
 *
 */
namespace DemoGameplayTags
{
// Item.Weapon.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon_NoWeapon);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon_Melee_OneHanded);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Weapon_Melee_TwoHanded);

// Item.Armor.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Armor);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Armor_Shield);

// Item.Consumable.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Consumable);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Consumable_Food);

// Movement.SpeedMode.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_SpeedMode_Walk);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_SpeedMode_Jog);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_SpeedMode_Sprint);

// Character states: State.[state]
// These can also be used as action tags.
// MUST sync with UStateManagerComponent::UStateManagerComponent() - std::call_once - AllStates
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_General);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Disabled);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Jump);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_ToggleCombat);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dodge);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block);

// Character actions: State.[state].[action]
// *.[subaction] is not allowed for now.
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_ToggleCombat_Enter);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_ToggleCombat_Exit);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack_Light);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack_Heavy);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attack_Charged);

// StateTree.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateTree_Tree_Main);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateTree_Event_TargetLost);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateTree_Event_TargetSensed);

// UI.PlayerMenu.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_PlayerMenu_Stats);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_PlayerMenu_Inventory);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_PlayerMenu_Equipment);

// UI.Action.Item.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Use);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Drop);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Unequip);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Action_Item_Cancel);
}