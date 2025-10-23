// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "StatsPageWidget.generated.h"

class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UStatsPageWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget Functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    //void ProcessPendingUpdates();

    void UpdateAllStats();

private:
    void BindToStatsComponent();

    void UpdateStatText(FGameplayTag StatTag, float OldValue, float NewValue);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentHealthText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentStaminaText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> STRText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DEXText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> INTText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> AttackText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DefenseText;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    TArray<FGameplayTag> PendingUpdateStatTags;

    TMap<FGameplayTag, TObjectPtr<UTextBlock>> StatTagToTextMap;
};