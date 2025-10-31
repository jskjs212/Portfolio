// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HAL/Platform.h" // HAL: Hardware Abstraction Layer

/**
 * Centralized ZOrder definitions for Demo UI widgets.
 */
namespace DemoZOrder
{
/* Core HUD */
// Used: DemoHUDWidget, MainMenuWidget (Outside of game session)
constexpr int32 HUD_Base{100};
// For QuestUpdate, ItemPickup, TutorialHint?
constexpr int32 HUD_Notification{110};
constexpr int32 HUD_Subtitle{120};

/* Main UI */
// Used: PlayerMenuWidget (In-game main player menu)
constexpr int32 PlayerMenu{200};

/* Non-modal */
// Used: ContextMenuWidget
constexpr int32 ContextMenu{300};

/* Modal */
constexpr int32 Modal_Base{400};

/* Event */
// Used: BossVictoryWidget
constexpr int32 Event_BossVictory{500};
// Used: YouDiedWidget
constexpr int32 Event_YouDied{510};

/* Cinematic */
constexpr int32 Cinematic{600};

/* System */
constexpr int32 System{8000};

/* Debug */
constexpr int32 Debug{9000};
} // namespace DemoZOrder