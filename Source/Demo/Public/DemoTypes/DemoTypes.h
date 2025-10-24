// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "DemoTypes.generated.h"

namespace DemoTeamID
{
const FGenericTeamId Player{1};
const FGenericTeamId Enemy{2};
const FGenericTeamId Neutral{3};
} // namespace DemoTeamID

// Consider adding: Droppable, Blocked, Using, Disabled, etc.
UENUM()
enum class ECursorState : uint8
{
    Default UMETA(DisplayName = "Default"),
    Hovering UMETA(DisplayName = "Hovering"),
    Dragging UMETA(DisplayName = "Dragging"),
    MAX UMETA(Hidden)
};

USTRUCT()
struct FCharacterWeaponKey
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (Categories = "Character"))
    FGameplayTag CharacterTag;

    UPROPERTY(EditAnywhere, meta = (Categories = "Item.Weapon"))
    FGameplayTag WeaponTag;

    bool operator==(const FCharacterWeaponKey& Other) const
    {
        return CharacterTag == Other.CharacterTag && WeaponTag == Other.WeaponTag;
    }
};

FORCEINLINE uint32 GetTypeHash(const FCharacterWeaponKey& Key)
{
    return HashCombine(GetTypeHash(Key.CharacterTag), GetTypeHash(Key.WeaponTag));
}