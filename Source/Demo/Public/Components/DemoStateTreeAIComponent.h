// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"
#include "DemoStateTreeAIComponent.generated.h"

/**
 *
 */
UCLASS()
class DEMO_API UDemoStateTreeAIComponent : public UStateTreeAIComponent
{
    GENERATED_BODY()

public:
    /**
     * Add a linked state tree override.
     * The override won't be set if it doesn't use the StateTreeComponentSchema schema.
     * The implementation of UStateTreeComponent::AddLinkedStateTreeOverrides() has missing "NOT boolean" in line: 507.
     * Fixed in 5.7.0 (https://github.com/EpicGames/UnrealEngine/commit/f602e71240d38286955b13232cdbb436f3186ada)
     * Copied and applied the fix here.
     */
    UFUNCTION(BlueprintCallable, Category = "Gameplay|StateTree")
    void AddLinkedStateTreeOverrides_Fixed(const FGameplayTag StateTag, FStateTreeReference StateTreeReference);
};