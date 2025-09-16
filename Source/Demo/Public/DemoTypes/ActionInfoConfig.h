// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ActionInfoConfig.generated.h"

class UAnimMontage;

USTRUCT()
struct FActionInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float DamageMultiplier{0.0f};

    UPROPERTY(EditAnywhere)
    float StaminaCost{0.0f};

    UPROPERTY(EditAnywhere)
    float PlayRate{1.0f};

    UPROPERTY(EditAnywhere)
    FName StartSection;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UAnimMontage> AnimMontage;
};

USTRUCT()
struct FActionInfos
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<FActionInfo> Array;
};

/**
 *
 */
UCLASS()
class DEMO_API UActionInfoConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    const TArray<FActionInfo>* GetActionInfoArray(FGameplayTag InAction) const
    {
        if (const FActionInfos* ActionInfos = ActionInfoMap.Find(InAction))
        {
            return &ActionInfos->Array;
        }
        return nullptr;
    }

private:
    // @TODO - Validation: Every array should have at least one element.
    // Map of { ActionTag, Array of ActionInfo }
    UPROPERTY(EditAnywhere, meta = (Categories = "State"))
    TMap<FGameplayTag, FActionInfos> ActionInfoMap;
};