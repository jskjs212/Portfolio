// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameplayTagContainer.h"
#include "BossTriggerBox.generated.h"

enum class EBossEncounterEndReason : uint8;
class UBossEncounterComponent;
class UBoxComponent;
class UStaticMeshComponent;

/**
 * Trigger box that starts a boss encounter when the player enters.
 * Blocks entrance during the encounter.
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

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> EntranceBlockerComponent;

    ////////////////////////////////////////////////////////
    //        Functions
    ////////////////////////////////////////////////////////
public:
    ABossTriggerBox();

protected:
    virtual void BeginPlay() override;

private:
    void SetEntranceBlocked(bool bBlocked);

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    UFUNCTION()
    void HandleBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

    UFUNCTION()
    void HandleEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

    void HandleEncounterStarted(APawn* InInstigator);

    void HandleEncounterEnded(EBossEncounterEndReason Reason);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization")
    TWeakObjectPtr<APawn> BossPawn;

    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Categories = "Sound.Music"))
    FGameplayTag BossMusicTag;

    // Relative location
    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (MakeEditWidget = "true"))
    FVector InstigatorTeleportLocation;
};