// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interfaces/TargetInterface.h"
#include "StateTreeReference.h"
#include "AICharacter.generated.h"

class UAIStatusWidget;
class UWidgetComponent;

/**
 * AICharacter class that should be inherited by enemy or NPC character classes.
 */
UCLASS()
class DEMO_API AAICharacter : public ABaseCharacter, public ITargetInterface
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    // Health bar, nameplate (and maybe CC, level, casting, charging)
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UWidgetComponent> AIStatusWidgetComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UWidgetComponent> LockOnMarkerWidgetComponent;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    AAICharacter();

protected:
    virtual void BeginPlay() override;

    virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    ////////////////////////////////////////////////////////
    //        AICharacter functions
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE bool IsBoss() const { return bIsBoss; }

    ////////////////////////////////////////////////////////
    //        Combat interface
    ////////////////////////////////////////////////////////
public:
    virtual float PerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex = false) override;

    virtual bool CanReceiveDamageFrom(const AActor* Attacker) const override;

    // Sets control rotation to face the target actor.
    virtual FRotator GetDesiredControlRotation() const override;

    ////////////////////////////////////////////////////////
    //        Target interface
    ////////////////////////////////////////////////////////
public:
    virtual void OnTargeted(bool bIsTargeted) override;

    virtual bool CanBeTargeted() const override;

    virtual FVector GetTargetPointLocation() const override;

    FORCEINLINE const FStateTreeReference& GetStateTreeRefOverride() const { return StateTreeRefOverride; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    bool bIsBoss{false};

    UPROPERTY(EditAnywhere, Category = "Initialization|AI")
    FName LockOnSocketName{TEXT("Head")};

    UPROPERTY(EditAnywhere, Category = "Initialization|AI|StateTreeOverride")
    FStateTreeReference StateTreeRefOverride;
};