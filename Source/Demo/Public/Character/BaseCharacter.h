// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Interfaces/CombatInterface.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;
class UCombatComponent;
class UEquipmentComponent;
class UStateManagerComponent;
class UStatsComponent;

UCLASS()
class DEMO_API ABaseCharacter : public ACharacter, public ICombatInterface
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UEquipmentComponent> EquipmentComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStateManagerComponent> StateManager;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStatsComponent> StatsComponent;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    ABaseCharacter();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        Character functions
    ////////////////////////////////////////////////////////
protected:
    bool CanPerformJump() const;

    virtual void Jump() override;

    virtual void Landed(const FHitResult& Hit) override;

    // CharacterMovementComponent's movement mode
    virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

    void DestroyCharacter();

    void EnableRagdoll();

    void HandleDeath();

    void HandleCurrentResourceStatChanged(FGameplayTag StatTag, float OldValue, float NewValue);

    void HandleStateBegan(FGameplayTag NewState);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void SetMovementSpeedMode(FGameplayTag NewSpeedMode);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    /* Movement */
    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed{180.f};
    UPROPERTY(EditAnywhere, Category = "Movement")
    float JogSpeed{450.f};
    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeed{800.f};

    UPROPERTY(EditAnywhere, Category = "Movement")
    float JumpStaminaCost{10.f};

    // Should not modify directly, use SetMovementSpeedMode instead.
    FGameplayTag MovementSpeedMode;

    /* Animation */
    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimMontage> DeathMontage;
};