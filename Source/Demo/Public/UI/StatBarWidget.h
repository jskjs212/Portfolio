// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "StatBarWidget.generated.h"

class UProgressBar;
class UStatsComponent;
class UTexture2D;

/**
 * Should call BindToStatsComponent() after created.
 * Only Stat.Resource type is supported for now.
 */
UCLASS()
class DEMO_API UStatBarWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    virtual void NativePreConstruct() override;

public:
    // Unbind from previous StatsComponent if already bound.
    void BindToStatsComponent(AActor* InActor, FGameplayTag InStatTag);

    void UpdateStatBar();

private:
    UFUNCTION()
    void HandleCurrentStatChanged(FGameplayTag InStatTag, float OldValue, float NewValue)
    {
        if (StatTag == InStatTag)
        {
            UpdateStatBar();
        }
    }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> StatBar;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    FGameplayTag StatTag;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    FLinearColor StatBarFillColor{FLinearColor::White};

    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<UTexture2D> StatBarFillImage;

    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<UTexture2D> StatBarBackgroundImage;

    // References
    FDelegateHandle StatChangedDelegateHandle;
    TWeakObjectPtr<UStatsComponent> CachedStatsComponent;
};