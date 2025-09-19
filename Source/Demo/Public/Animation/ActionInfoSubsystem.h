// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DemoTypes/ActionInfoConfig.h"
#include "GameplayTagContainer.h"
#include "ActionInfoSubsystem.generated.h"

/**
 * Manages ActionInfoConfigs.
 * You should configure the DefaultActionInfoEntries (array of FActionInfoEntry) in the Demo Project Settings in the editor.
 */
UCLASS()
class DEMO_API UActionInfoSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

    const UActionInfoConfig* GetActionInfoConfig(FGameplayTag IdentityTag, FGameplayTag WeaponTag) const;

private:
    // { Key{ IdentityTag, WeaponTag }, ActionInfoConfig }
    UPROPERTY(Transient)
    TMap<FActionInfoKey, TObjectPtr<UActionInfoConfig>> ActionInfoMap;
};