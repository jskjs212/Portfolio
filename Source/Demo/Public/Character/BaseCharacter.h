// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Interfaces/CombatInterface.h"
#include "BaseCharacter.generated.h"

struct FActionInfo;
class UActionInfoConfig;
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
    /* Movement */
    bool CanPerformJump() const;

    virtual void Jump() override;

    virtual void Landed(const FHitResult& Hit) override;

    // CharacterMovementComponent's movement mode
    virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

    /* Animation */
    // Update CurrentActionInfo according to the weapon.
    void UpdateActionInfo(FGameplayTag WeaponTag);

    /* Death */
    void DestroyCharacter();

    void EnableRagdoll();

    void HandleDeath();

    /* Event handlers */
    void HandleCurrentResourceStatChanged(FGameplayTag StatTag, float OldValue, float NewValue);

    void HandleStateBegan(FGameplayTag NewState);

    ////////////////////////////////////////////////////////
    //        Combat interface
    ////////////////////////////////////////////////////////
public:
    // @return Duration of the action's AnimMontage, or 0.f if failed to perform action.
    virtual float PerformAction(FGameplayTag InAction, int32 MontageIndex, bool bUseRandomIndex = false) override;

private:
    // Helper function
    // @return nullptr if can't perform the action.
    const FActionInfo* CanPerformAction(FGameplayTag InAction, int32 MontageIndex, bool bUseRandomIndex) const;

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
    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (Categories = "Identity"))
    FGameplayTag IdentityTag;

    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimMontage> DeathMontage;

    UPROPERTY(VisibleAnywhere, Transient, Category = "Combat")
    TObjectPtr<const UActionInfoConfig> CurrentActionInfo;
};