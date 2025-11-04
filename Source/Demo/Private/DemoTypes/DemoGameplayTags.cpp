// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoTypes/DemoGameplayTags.h"

namespace DemoGameplayTags
{
// IMC.*
UE_DEFINE_GAMEPLAY_TAG(IMC_Modifier, "IMC.Modifier");
UE_DEFINE_GAMEPLAY_TAG(IMC_PlayerCharacter_Default, "IMC.PlayerCharacter.Default");
UE_DEFINE_GAMEPLAY_TAG(IMC_PlayerController_Default, "IMC.PlayerController.Default");
UE_DEFINE_GAMEPLAY_TAG(IMC_PlayerController_AfterDeath, "IMC.PlayerController.AfterDeath");

// Input.*
UE_DEFINE_GAMEPLAY_TAG(Input_Look, "Input.Look");
UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
UE_DEFINE_GAMEPLAY_TAG(Input_Jump, "Input.Jump");
UE_DEFINE_GAMEPLAY_TAG(Input_Walk, "Input.Walk");
UE_DEFINE_GAMEPLAY_TAG(Input_Sprint, "Input.Sprint");
UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
UE_DEFINE_GAMEPLAY_TAG(Input_LightAttack, "Input.LightAttack");
UE_DEFINE_GAMEPLAY_TAG(Input_HeavyAttack, "Input.HeavyAttack");
UE_DEFINE_GAMEPLAY_TAG(Input_Block, "Input.Block");
UE_DEFINE_GAMEPLAY_TAG(Input_Dodge, "Input.Dodge");
UE_DEFINE_GAMEPLAY_TAG(Input_ToggleLockOn, "Input.ToggleLockOn");
UE_DEFINE_GAMEPLAY_TAG(Input_ShowPlayerMenu, "Input.ShowPlayerMenu");
UE_DEFINE_GAMEPLAY_TAG(Input_ToggleHelpText, "Input.ToggleHelpText");
UE_DEFINE_GAMEPLAY_TAG(Input_GoToMainMenu, "Input.GoToMainMenu");
UE_DEFINE_GAMEPLAY_TAG(Input_Test1, "Input.Test1");
UE_DEFINE_GAMEPLAY_TAG(Input_Test2, "Input.Test2");
UE_DEFINE_GAMEPLAY_TAG(Input_Escape, "Input.Escape");

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

// Setting.*
UE_DEFINE_GAMEPLAY_TAG(Setting_Graphics, "Setting.Graphics");
UE_DEFINE_GAMEPLAY_TAG(Setting_Audio, "Setting.Audio");
UE_DEFINE_GAMEPLAY_TAG(Setting_Audio_MuteWhenUnfocused, "Setting.Audio.MuteWhenUnfocused");
UE_DEFINE_GAMEPLAY_TAG(Setting_Controls, "Setting.Controls");
UE_DEFINE_GAMEPLAY_TAG(Setting_Controls_WalkInputToggle, "Setting.Controls.WalkInputToggle");
UE_DEFINE_GAMEPLAY_TAG(Setting_Controls_SprintInputToggle, "Setting.Controls.SprintInputToggle");
UE_DEFINE_GAMEPLAY_TAG(Setting_System, "Setting.System");

// Stat.*
UE_DEFINE_GAMEPLAY_TAG(Stat_Resource_Health, "Stat.Resource.Health");
UE_DEFINE_GAMEPLAY_TAG(Stat_Resource_Stamina, "Stat.Resource.Stamina");
UE_DEFINE_GAMEPLAY_TAG(Stat_Resource_Mana, "Stat.Resource.Mana");
UE_DEFINE_GAMEPLAY_TAG(Stat_Primary_STR, "Stat.Primary.STR");
UE_DEFINE_GAMEPLAY_TAG(Stat_Primary_DEX, "Stat.Primary.DEX");
UE_DEFINE_GAMEPLAY_TAG(Stat_Primary_INT, "Stat.Primary.INT");
UE_DEFINE_GAMEPLAY_TAG(Stat_Derived_Attack, "Stat.Derived.Attack");
UE_DEFINE_GAMEPLAY_TAG(Stat_Derived_Defense, "Stat.Derived.Defense");

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
UE_DEFINE_GAMEPLAY_TAG(State_Disabled_HitstunFront, "State.Disabled.HitstunFront");
UE_DEFINE_GAMEPLAY_TAG(State_Disabled_HitstunBack, "State.Disabled.HitstunBack");
UE_DEFINE_GAMEPLAY_TAG(State_Disabled_HitstunLeft, "State.Disabled.HitstunLeft");
UE_DEFINE_GAMEPLAY_TAG(State_Disabled_HitstunRight, "State.Disabled.HitstunRight");
UE_DEFINE_GAMEPLAY_TAG(State_ToggleCombat_Enter, "State.ToggleCombat.Enter");
UE_DEFINE_GAMEPLAY_TAG(State_ToggleCombat_Exit, "State.ToggleCombat.Exit");
UE_DEFINE_GAMEPLAY_TAG(State_Attack_Light, "State.Attack.Light");
UE_DEFINE_GAMEPLAY_TAG(State_Attack_Heavy, "State.Attack.Heavy");
UE_DEFINE_GAMEPLAY_TAG(State_Attack_Charged, "State.Attack.Charged");

// StateTree.*
UE_DEFINE_GAMEPLAY_TAG(StateTree_Tree_Main, "StateTree.Tree.Main");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Tree_Peaceful, "StateTree.Tree.Peaceful");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Tree_Aggressive, "StateTree.Tree.Aggressive");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_Alert, "StateTree.Event.Alert");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_TargetLost, "StateTree.Event.TargetLost");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_TargetSensed, "StateTree.Event.TargetSensed");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_Death, "StateTree.Event.Death");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_Respawn, "StateTree.Event.Respawn");
UE_DEFINE_GAMEPLAY_TAG(StateTree_Event_Ressurect, "StateTree.Event.Resurrect");

// UI.Action.Item.*
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Add, "UI.Action.Item.Add");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Remove, "UI.Action.Item.Remove");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Action_Item_Use, "UI.Action.Item.Use", "Equip, consume, etc.")
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Drop, "UI.Action.Item.Drop");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Swap, "UI.Action.Item.Swap");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Unequip, "UI.Action.Item.Unequip");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_UnequipAndDrop, "UI.Action.Item.UnequipAndDrop");
UE_DEFINE_GAMEPLAY_TAG(UI_Action_Item_Cancel, "UI.Action.Item.Cancel");

// UI.PlayerMenu.*
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Stats, "UI.PlayerMenu.Stats");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Inventory, "UI.PlayerMenu.Inventory");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Equipment, "UI.PlayerMenu.Equipment");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Settings, "UI.PlayerMenu.Settings");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_SystemMenu, "UI.PlayerMenu.SystemMenu");
} // namespace DemoGameplayTags