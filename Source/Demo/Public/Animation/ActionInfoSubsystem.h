// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "ActionInfoSubsystem.generated.h"

class UActionInfoConfig;

USTRUCT()
struct FActionInfoKey
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (Categories = "Identity"))
    FGameplayTag IdentityTag;

    UPROPERTY(EditAnywhere, meta = (Categories = "Item.Weapon"))
    FGameplayTag WeaponTag;

    bool operator==(const FActionInfoKey& Other) const
    {
        return IdentityTag == Other.IdentityTag && WeaponTag == Other.WeaponTag;
    }
};

/**
 * Helper struct to make it easier to edit TMap's entries in the editor.
 */
USTRUCT()
struct FActionInfoEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FActionInfoKey Key;

    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<UActionInfoConfig> ActionInfo;
};

FORCEINLINE uint32 GetTypeHash(const FActionInfoKey& Key)
{
    return HashCombine(GetTypeHash(Key.IdentityTag), GetTypeHash(Key.WeaponTag));
}

/**
 *
 */
UCLASS(Abstract)
class DEMO_API UActionInfoSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    const UActionInfoConfig* GetActionInfoConfig(FGameplayTag IdentityTag, FGameplayTag WeaponTag) const;

private:
    // Configure this array of entries in the editor.
    UPROPERTY(EditAnywhere)
    TArray<FActionInfoEntry> ActionInfoEntries;

    // { Key{ IdentityTag, WeaponTag }, ActionInfoConfig }
    UPROPERTY(Transient)
    TMap<FActionInfoKey, TObjectPtr<UActionInfoConfig>> ActionInfoMap;
};