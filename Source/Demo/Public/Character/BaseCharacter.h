// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemoTypes/ItemTypes.h"
#include "DemoTypes/StatsTypes.h"
#include "GameplayTagContainer.h"
#include "Interfaces/CombatInterface.h"
#include "BaseCharacter.generated.h"

struct FActionInfo;
struct FWeaponData;
class UActionInfoConfig;
class UAnimMontage;
class UCollisionComponent;
class UCombatComponent;
class UEquipmentComponent;
class UParticleSystem;
class USoundBase;
class UStateManagerComponent;
class UStatsComponent;

/**
 * If input action is not working, check if AnimMontage has AnimNotify_ResetState.
 */
UCLASS()
class DEMO_API ABaseCharacter : public ACharacter, public ICombatInterface
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCollisionComponent> CollisionComponent;

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

    virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

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
    void UpdateAnimationData(FGameplayTag WeaponTag);

    /* Hit */
    // Play hit sound, particle, and animation.
    void PlayPointHitEffects(const FPointDamageEvent& PointDamageEvent, const AController* EventInstigator);

    /* Death */
    bool IsDead() const;

    void DestroyCharacter();

    void EnableRagdoll();

    void HandleDeath();

    /* Event handlers */
    void HandleCurrentResourceStatChanged(FGameplayTag StatTag, float OldValue, float NewValue);

    void HandleStateBegan(FGameplayTag NewState);

    virtual void HandleWeaponChanged(const FWeaponData* WeaponData);

    ////////////////////////////////////////////////////////
    //        Combat interface
    ////////////////////////////////////////////////////////
public:
    // @param bIgnoreState: If true, ignore current state.
    // @return Duration of the action's AnimMontage, or 0.f if failed to perform action.
    virtual float PerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex = false) override;

    virtual bool IsInAction(FGameplayTag InAction) const override;

    virtual bool CanReceiveDamage() const override;

    virtual float CalculateDamage(EAttackCollisionType InType) const override;

    virtual int32 GetActionInfoCount(FGameplayTag InAction) const override;

    virtual FRotator GetDesiredControlRotation() const override;

    virtual FRotator GetDesiredInputRotation() const override;

private:
    // Helper function that validates all conditions for PerformAction.
    // @return nullptr if can't perform the action.
    const FActionInfo* CanPerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex) const;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void SetMovementSpeedMode(FGameplayTag NewSpeedMode);

    void SetOrientRotationToMovement(bool bOrient);

    ////////////////////////////////////////////////////////
    //        Variables - Hit
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "Initialization|Animation")
    TObjectPtr<UAnimMontage> HitReactFrontMontage;

    UPROPERTY(EditAnywhere, Category = "Initialization|Animation")
    TObjectPtr<UAnimMontage> HitReactBackMontage;

    UPROPERTY(EditAnywhere, Category = "Initialization|Animation")
    TObjectPtr<UAnimMontage> HitReactLeftMontage;

    UPROPERTY(EditAnywhere, Category = "Initialization|Animation")
    TObjectPtr<UAnimMontage> HitReactRightMontage;

    UPROPERTY(EditAnywhere, Category = "Initialization|Animation")
    TObjectPtr<UAnimMontage> DeathMontage;

    UPROPERTY(EditAnywhere, Category = "Initialization|Hit")
    TObjectPtr<USoundBase> HitSound;

    UPROPERTY(EditAnywhere, Category = "Initialization|Hit")
    TObjectPtr<UParticleSystem> HitParticle;

    UPROPERTY(EditAnywhere, Category = "Initialization|Hit")
    FTransform HitParticleTransform;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    /* Stats */
    // @TODO - Use data table or config file
    UPROPERTY(EditAnywhere, Category = "Initialization|Stats", meta = (Categories = "Stat"))
    TMap<FGameplayTag, FResourceStat> ResourceStats;

    /* Equipment */
    UPROPERTY(EditAnywhere, Category = "Initialization|Equipment")
    TArray<FItemSlot> StartingItems;

    /* Movement */
    UPROPERTY(EditAnywhere, Category = "Initialization|Movement")
    float WalkSpeed{180.f};
    UPROPERTY(EditAnywhere, Category = "Initialization|Movement")
    float JogSpeed{450.f};
    UPROPERTY(EditAnywhere, Category = "Initialization|Movement")
    float SprintSpeed{800.f};

    UPROPERTY(EditAnywhere, Category = "Initialization|Movement")
    float JumpStaminaCost{10.f};

    // Should not modify directly, use SetMovementSpeedMode instead.
    FGameplayTag MovementSpeedMode;

    /* Animation */
    UPROPERTY(EditAnywhere, Category = "Initialization|Character", meta = (Categories = "Character"))
    FGameplayTag CharacterTag;

    UPROPERTY(VisibleAnywhere, Transient, Category = "Combat")
    TObjectPtr<const UActionInfoConfig> CurrentActionInfo;

    // @TEST - Temporary
    UPROPERTY(EditAnywhere, Category = "Initialization|Collision", meta = (Categories = "State"))
    TMap<FGameplayTag, float> ActionToDamageMap;
};