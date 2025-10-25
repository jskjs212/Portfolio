// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "BossTriggerBox.generated.h"

class UBossEncounterComponent;

/**
 *
 */
UCLASS()
class DEMO_API ABossTriggerBox : public ATriggerBox
{
    GENERATED_BODY()

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBossEncounterComponent> BossEncounterComponent;

public:
    ABossTriggerBox();

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void HandleBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

    UFUNCTION()
    void HandleEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    TObjectPtr<APawn> BossPawn;
};