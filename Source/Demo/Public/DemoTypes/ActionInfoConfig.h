// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DemoTypes/DemoTypes.h"
#include "GameplayTagContainer.h"
#include "ActionInfoConfig.generated.h"

class UAnimMontage;

USTRUCT()
struct FActionInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    float DamageMultiplier{0.f};

    UPROPERTY(EditAnywhere)
    float StaminaCost{0.f};

    UPROPERTY(EditAnywhere)
    float PlayRate{1.f};

    UPROPERTY(EditAnywhere)
    FName StartSection;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UAnimMontage> AnimMontage;

    bool IsValid() const
    {
        if (DamageMultiplier < 0.f || StaminaCost < 0.f || PlayRate <= 0.f || !AnimMontage)
        {
            return false;
        }
        return true;
    }
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
    bool IsValid() const
    {
        for (const auto& [ActionTag, ActionInfos] : ActionInfoMap)
        {
            if (!ActionTag.IsValid() || ActionInfos.Array.Num() == 0)
            {
                return false;
            }

            for (const FActionInfo& ActionInfo : ActionInfos.Array)
            {
                if (!ActionInfo.IsValid())
                {
                    return false;
                }
            }
        }
        return true;
    }

    const TArray<FActionInfo>* GetActionInfoArray(FGameplayTag InAction) const
    {
        if (const FActionInfos* ActionInfos = ActionInfoMap.Find(InAction))
        {
            return &ActionInfos->Array;
        }
        return nullptr;
    }

private:
    // Map of { ActionTag, Array of ActionInfo }
    UPROPERTY(EditAnywhere, meta = (Categories = "State"))
    TMap<FGameplayTag, FActionInfos> ActionInfoMap;
};