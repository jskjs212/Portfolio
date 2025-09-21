// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DemoTypes/DemoTypes.h"
#include "GameplayTagContainer.h"
#include "AnimationDataSubsystem.generated.h"

class UActionInfoConfig;

/**
 * Manages ActionInfoConfigs and ItemAnimLayerClasses based on {CharacterTag, WeaponTag} keys.
 * You should configure the DataAssets in the Demo Project Settings in the editor.
 */
UCLASS()
class DEMO_API UAnimationDataSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    const UActionInfoConfig* GetActionInfoConfig(FGameplayTag CharacterTag, FGameplayTag WeaponTag) const
    {
        const FCharacterWeaponKey Key = FCharacterWeaponKey{CharacterTag, WeaponTag};
        const TObjectPtr<const UActionInfoConfig>* ActionInfoPtr = ActionInfoMap.Find(Key);
        return ActionInfoPtr ? *ActionInfoPtr : nullptr;
    }

    const TSubclassOf<UAnimInstance> GetItemAnimLayerClass(FGameplayTag CharacterTag, FGameplayTag WeaponTag) const
    {
        const FCharacterWeaponKey Key = FCharacterWeaponKey{CharacterTag, WeaponTag};
        const TSubclassOf<UAnimInstance>* ItemAnimLayerPtr = ItemAnimLayerMap.Find(Key);
        return ItemAnimLayerPtr ? *ItemAnimLayerPtr : nullptr;
    }

private:
    // {Key{CharacterTag, WeaponTag}, ActionInfoConfig}
    UPROPERTY(Transient)
    TMap<FCharacterWeaponKey, TObjectPtr<const UActionInfoConfig>> ActionInfoMap;

    // {Key{CharacterTag, WeaponTag}, ItemAnimLayerClass}
    UPROPERTY(Transient)
    TMap<FCharacterWeaponKey, TSubclassOf<UAnimInstance>> ItemAnimLayerMap;
};