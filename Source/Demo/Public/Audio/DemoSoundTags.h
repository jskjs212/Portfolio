// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"
#include "Containers/Array.h"
#include "GameplayTagContainer.h"

/**
 *
 */
namespace DemoSoundTags
{
// Sound.Master
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Master);

// Sound.Music.*
// Background music
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Music);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Music_Boss_Default);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Music_Exploration);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Music_MainMenu);

// Sound.SFX.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SFX);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SFX_Game_Hit_Default);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SFX_Game_Hit_Metal);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SFX_Item_PickUp);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SFX_Item_Equip);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SFX_Item_Unequip);

// Sound.UI.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_ItemSlot_Hover);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_PlayerMenu_Open);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_PlayerMenu_Close);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_TabButton_Click);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_TabButton_Hover);

// Sound.Voice.*
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Voice);
DEMO_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Voice_ElfArden_Death);

// Defines audio categories for Demo's audio system.
// MUST sync with UDemoAudioSubsystem::AudioMap.
FORCEINLINE const TArray<FGameplayTag>& GetCategories()
{
    static const TArray<FGameplayTag> Categories{
        DemoSoundTags::Music,
        DemoSoundTags::SFX,
        DemoSoundTags::UI,
        DemoSoundTags::Voice
    };
    return Categories;
}

// Find category from sound tag.
// e.g. Sound.SFX.UI.Click -> Sound.SFX
// @return EmptyTag if not found.
FORCEINLINE FGameplayTag GetCategory(const FGameplayTag SoundTag)
{
    for (const FGameplayTag& Category : GetCategories())
    {
        if (SoundTag.MatchesTag(Category))
        {
            return Category;
        }
    }
    return FGameplayTag::EmptyTag;
}
} // namespace DemoSoundTags