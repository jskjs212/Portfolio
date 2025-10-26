// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameplayTagContainer.h"
#include "BossTriggerBox.generated.h"

class UBossEncounterComponent;

/**
 *
 */
UCLASS()
class DEMO_API ABossTriggerBox : public ATriggerBox
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBossEncounterComponent> BossEncounterComponent;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    ABossTriggerBox();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    UFUNCTION()
    void HandleBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

    UFUNCTION()
    void HandleEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    TWeakObjectPtr<APawn> BossPawn;

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Categories = "Sound.Music"))
    FGameplayTag BossMusicTag;
};