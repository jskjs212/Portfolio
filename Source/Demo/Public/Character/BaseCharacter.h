// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemoTypes/ItemTypes.h"
#include "GameplayTagContainer.h"
#include "Interfaces/CombatInterface.h"
#include "BaseCharacter.generated.h"

struct FActionInfo;
struct FWeaponData;
class UActionInfoConfig;
class UAnimMontage;
class UAttackCollisionComponent;
class UCombatComponent;
class UDemoPawnData;
class UEquipmentComponent;
class UParticleSystem;
class USoundBase;
class UStateManagerComponent;
class UStatsComponent;

DECLARE_DELEGATE_OneParam(FOnBlockingStateChanged, bool /* bNewBlocking */);

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
    TObjectPtr<UAttackCollisionComponent> AttackCollisionComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UEquipmentComponent> EquipmentComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStateManagerComponent> StateManager;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStatsComponent> StatsComponent;

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnBlockingStateChanged OnBlockingStateChanged;

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
public:
    FORCEINLINE bool IsDead() const { return bIsDead; }

protected:
    /* Init */
    void InitCharacter();

    /* Movement */
    bool CanPerformJump() const;

    virtual void Jump() override;

    virtual void Landed(const FHitResult& Hit) override;

    /* Combat */
    void SetBlockingState(bool bNewBlocking);
    void StartBlocking();
    void StopBlocking();

    // CharacterMovementComponent's movement mode
    virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

    /* Animation */
    // Update CurrentActionInfo according to the weapon.
    void UpdateAnimationData(FGameplayTag WeaponTag);

    /* Hit */
    // Play hit sound, particle, and animation.
    void PlayPointHitEffects(const FPointDamageEvent& PointDamageEvent, const AController* EventInstigator);

    /* Death */
    void DestroyCharacter();

    void EnableRagdoll();

    virtual void HandleDeath();

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

    virtual void EnableIFrame(bool bEnable) override { bIFrameEnabled = bEnable; }

    virtual bool IsInAction(FGameplayTag InAction) const override;

    virtual bool CanReceiveDamageFrom(const AActor* Attacker) const override;

    virtual float CalculateDamage(EAttackCollisionType InType) const override;

    virtual int32 GetActionInfoCount(FGameplayTag InAction) const override;

    virtual FRotator GetDesiredControlRotation() const override;

    virtual FRotator GetDesiredInputRotation() const override;

private:
    // Helper function that validates all conditions for PerformAction.
    // @return nullptr if can't perform the action.
    const FActionInfo* PerformAction_Validate(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex) const;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    bool IsBlocking() const { return bIsBlocking; }

    FORCEINLINE FText GetCharacterDisplayName() const { return CharacterDisplayName; }

    UFUNCTION(BlueprintCallable, Category = "Movement", meta = (Categories = "Movement.SpeedMode"))
    virtual void SetMovementSpeedMode(FGameplayTag NewSpeedMode);

    void SetOrientRotationToMovement(bool bOrient);

    ////////////////////////////////////////////////////////
    //        Variables - Core
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "Initialization|Character")
    TObjectPtr<const UDemoPawnData> PawnData;

    UPROPERTY(VisibleAnywhere)
    FGameplayTag CharacterTag;

    UPROPERTY(VisibleAnywhere)
    FText CharacterDisplayName;

    UPROPERTY(VisibleAnywhere, Transient, Category = "Combat")
    TObjectPtr<const UActionInfoConfig> CurrentActionInfo;

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

    UPROPERTY(EditAnywhere, Category = "Initialization|Animation")
    float DeathDestroyDelay{5.f};

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

    /* Combat */
    bool bIsDead{false};

    // Blocking with a shield.
    bool bIsBlocking{false};

    // Invincible
    bool bIFrameEnabled{false};
};