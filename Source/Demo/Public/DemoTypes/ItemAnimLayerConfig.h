// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DemoTypes/DemoTypes.h"
#include "GameplayTagContainer.h"
#include "ItemAnimLayerConfig.generated.h"

class UAnimInstance;

/**
 *
 */
UCLASS()
class DEMO_API UItemAnimLayerConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    // Each ItemAnimLayerClass should be associated with a unique {CharacterTag, WeaponTag} pair.
    UPROPERTY(EditDefaultsOnly, meta = (Categories = "Item.Weapon"))
    TMap<FCharacterWeaponKey, TSubclassOf<UAnimInstance>> ItemAnimLayerMap;
};