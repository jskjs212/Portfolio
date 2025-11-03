// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "HAL/Platform.h"
#include "Templates/Tuple.h"

class UInputMappingContext;

// Helper functions for Demo input system.
namespace Demo::Input
{
// Find and load an IMC from the DemoInputMappingCollection in the project settings.
// @return {IMC, Priority} if found, nullptr otherwise.
TPair<const UInputMappingContext*, int32> FindInputMappingContext(FGameplayTag ContextTag);
} // namespace Demo::Input