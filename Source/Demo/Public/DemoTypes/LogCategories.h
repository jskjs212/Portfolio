// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDemoGame, Log, All); // General game logs

DECLARE_LOG_CATEGORY_EXTERN(LogAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogAttributes, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCombat, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEquipment, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSettings, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogStateManager, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogTEST, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All);

/* Helper macros */
// Add [FuncName] prefix to log messages.
#define DemoLOG_F(CategoryName, Verbosity, Format, ...) UE_LOG(CategoryName, Verbosity, TEXT("[%hs] ") Format, __func__, ##__VA_ARGS__);

// Add [ClassName::FuncName] prefix to log messages.
#define DemoLOG_CF(CategoryName, Verbosity, Format, ...) UE_LOG(CategoryName, Verbosity, TEXT("[%hs] ") Format, __FUNCTION__, ##__VA_ARGS__);

// __FUNCTION__: Class::Func
// __func__: Func
// __LINE__: Line number
// __FILE__: File name
// __DATE__: Date
// __TIME__: Time