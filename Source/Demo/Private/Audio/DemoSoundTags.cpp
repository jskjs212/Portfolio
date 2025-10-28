// Fill out your copyright notice in the Description page of Project Settings.

#include "Audio/DemoSoundTags.h"

namespace DemoSoundTags
{
// Sound.Master
UE_DEFINE_GAMEPLAY_TAG(Master, "Sound.Master");

// Sound.Music.*
UE_DEFINE_GAMEPLAY_TAG(Music, "Sound.Music");
UE_DEFINE_GAMEPLAY_TAG(Music_Boss_Default, "Sound.Music.Boss.Default");
UE_DEFINE_GAMEPLAY_TAG(Music_Exploration, "Sound.Music.Exploration");
UE_DEFINE_GAMEPLAY_TAG(Music_MainMenu, "Sound.Music.MainMenu");

// Sound.SFX.*
UE_DEFINE_GAMEPLAY_TAG(SFX, "Sound.SFX");
UE_DEFINE_GAMEPLAY_TAG(SFX_Game_Hit_Default, "Sound.SFX.Game.Hit.Default");
UE_DEFINE_GAMEPLAY_TAG(SFX_Game_Hit_Metal, "Sound.SFX.Game.Hit.Metal");
UE_DEFINE_GAMEPLAY_TAG(SFX_Item_PickUp, "Sound.SFX.Item.PickUp");
UE_DEFINE_GAMEPLAY_TAG(SFX_Item_Equip, "Sound.SFX.Item.Equip");
UE_DEFINE_GAMEPLAY_TAG(SFX_Item_Unequip, "Sound.SFX.Item.Unequip");

// Sound.UI.*
UE_DEFINE_GAMEPLAY_TAG(UI, "Sound.UI");
UE_DEFINE_GAMEPLAY_TAG(UI_ItemSlot_Hover, "Sound.UI.ItemSlot.Hover");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Open, "Sound.UI.PlayerMenu.Open");
UE_DEFINE_GAMEPLAY_TAG(UI_PlayerMenu_Close, "Sound.UI.PlayerMenu.Close");
UE_DEFINE_GAMEPLAY_TAG(UI_TabButton_Click, "Sound.UI.TabButton.Click");
UE_DEFINE_GAMEPLAY_TAG(UI_TabButton_Hover, "Sound.UI.TabButton.Hover");

// Sound.Voice.*
UE_DEFINE_GAMEPLAY_TAG(Voice, "Sound.Voice");
UE_DEFINE_GAMEPLAY_TAG(Voice_ElfArden_Death, "Sound.Voice.ElfArden.Death");
} // namespace DemoSoundTags