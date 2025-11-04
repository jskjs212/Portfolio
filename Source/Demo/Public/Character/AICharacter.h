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

    // Show AIStatus for a while.
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // Report damage event to AI perception system.
    virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    ////////////////////////////////////////////////////////
    //        Character functions
    ////////////////////////////////////////////////////////
protected:
    virtual void HandleDeath() override;

    void HideAIStatusWidget();

    ////////////////////////////////////////////////////////
    //        Combat interface
    ////////////////////////////////////////////////////////
public:
    // Stop current AI movement if the new state doesn't allow.
    virtual float PerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex = false) override;

    virtual bool CanReceiveDamageFrom(const AActor* Attacker) const override;

    // Sets control rotation to face the target actor.
    virtual FRotator GetDesiredControlRotation() const override;

    ////////////////////////////////////////////////////////
    //        Target interface
    ////////////////////////////////////////////////////////
public:
    virtual void OnTargeted(bool bNewIsTargeted) override;

    virtual bool CanBeTargeted() const override;

    virtual FVector GetTargetPointLocation() const override;

    FORCEINLINE const FStateTreeReference& GetStateTreeRefOverride_Peaceful() const { return StateTreeRefOverride_Peaceful; }

    FORCEINLINE const FStateTreeReference& GetStateTreeRefOverride_Aggressive() const { return StateTreeRefOverride_Aggressive; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    bool bIsBoss{false};

    bool bIsTargeted{false};

    FTimerHandle HideAIStatusTimerHandle;
    FTimerDelegate HideAIStatusTimerDelegate;

    UPROPERTY(EditAnywhere, Category = "Initialization|AI")
    float GetHitAIStatusDuration{5.f};

    /* StateTree */
    // Can be set in AIController's StateTreeAIComponent, but here we can manage per-character overrides.
    UPROPERTY(EditAnywhere, Category = "Initialization|AI|StateTreeOverride|Peaceful")
    FStateTreeReference StateTreeRefOverride_Peaceful;

    UPROPERTY(EditAnywhere, Category = "Initialization|AI|StateTreeOverride|Aggressive")
    FStateTreeReference StateTreeRefOverride_Aggressive;

    UPROPERTY(EditAnywhere, Category = "Initialization|AI")
    FName LockOnSocketName{TEXT("Head")};
};