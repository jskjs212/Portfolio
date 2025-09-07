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
 * Should call InitStatBar() after created.
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
    void InitStatBar(AActor* OwnerActor, FGameplayTag InStatTag);

    void UpdateStatBar();

private:
    UFUNCTION()
    void OnStatChanged(FGameplayTag InStatTag, float OldValue, float NewValue)
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

    UPROPERTY(EditAnywhere, Category = "Appearance|StatBar")
    FLinearColor StatBarFillColor{FLinearColor::White};

    UPROPERTY(EditAnywhere, Category = "Appearance|StatBar")
    TObjectPtr<UTexture2D> StatBarFillImage;

    UPROPERTY(EditAnywhere, Category = "Appearance|StatBar")
    TObjectPtr<UTexture2D> StatBarBackgroundImage;

    // Reference to the character's stats component.
    TObjectPtr<UStatsComponent> StatsComponent;
};