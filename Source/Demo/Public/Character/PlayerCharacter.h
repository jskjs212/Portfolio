// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "DemoTypes/DemoTypes.h"
#include "DemoTypes/ItemTypes.h" // @TEST
#include "GenericTeamAgentInterface.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class IInteractable;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInventoryComponent;
class USpringArmComponent;
class USoundBase;
class UTargetingComponent;

DECLARE_DELEGATE_OneParam(FOnInteractableFocused, IInteractable* /* NewFocusedInteractable */);

/**
 *
 */
UCLASS()
class DEMO_API APlayerCharacter : public ABaseCharacter, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UInventoryComponent> InventoryComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UTargetingComponent> TargetingComponent;

    ////////////////////////////////////////////////////////
    //        Delegates
    ////////////////////////////////////////////////////////
public:
    FOnInteractableFocused OnInteractableFocused;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    APlayerCharacter();

protected:
    virtual void BeginPlay() override;

    ////////////////////////////////////////////////////////
    //        PlayerCharacter functions
    ////////////////////////////////////////////////////////
private:
    void ConsumeSprintStamina();

    // Trace first visible interactable from camera.
    IInteractable* TraceForInteractables();

    /* Event handlers */
    virtual void HandleWeaponChanged(const FWeaponData* WeaponData) override;

    // Trace and broadcast if focused interactable changed.
    void HandleInteractable();

    void HandleTargetUpdated(AActor* NewActor);

    ////////////////////////////////////////////////////////
    //        Combat interface
    ////////////////////////////////////////////////////////
public:
    virtual bool CanReceiveDamageFrom(const AActor* Attacker) const override;

    virtual FRotator GetDesiredInputRotation() const override;

    ////////////////////////////////////////////////////////
    //        GenericTeamAgentInterface
    ////////////////////////////////////////////////////////
public:
    virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

    ////////////////////////////////////////////////////////
    //        Input functions
    ////////////////////////////////////////////////////////
public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    void Look(const FInputActionValue& Value);

    void Move(const FInputActionValue& Value);
    void MoveComplete(const FInputActionValue& Value);

    virtual void Jump() override;
    virtual void Landed(const FHitResult& Hit) override;

    void StartWalking();
    void StopWalking();

    void StartSprinting();
    void StopSprinting();

    void Interact();

    void ShowPlayerMenu();

    /* Combat */
    void LightAttack();

    void Dodge();

    void ToggleLockOn();

    void ToggleHelpText();

    // @TEST
    UFUNCTION(BlueprintNativeEvent)
    void Test1();

    UFUNCTION(BlueprintNativeEvent)
    void Test2();

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    // @check - Call in AnimInstance?
    FORCEINLINE float GetGroundSpeed() const { return GetVelocity().Size2D(); }

    virtual void SetMovementSpeedMode(FGameplayTag NewSpeedMode) override;

    ////////////////////////////////////////////////////////
    //        Input variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> WalkAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ShowPlayerMenuAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> LightAttackAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> BlockAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> DodgeAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ToggleLockOnAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> ToggleHelpTextAction;

    // @TEST
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> Test1Action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> Test2Action;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
public:
    // @TODO - Audio
    UPROPERTY(EditDefaultsOnly, Category = "Initialization")
    TObjectPtr<USoundBase> PickupSound;

private:
    FGenericTeamId TeamID{DemoTeamID::Player};

    /* Movement */
    // Options: true = toggle, false = hold
    bool bIsWalkInputTogglesWalk{true};
    bool bIsSprintInputTogglesSprint{false};

    FVector2D CachedMoveInputAxis;

    // Sprint
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintStaminaCostPerSecond{10.f};
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintStaminaInterval{0.1f};

    FTimerHandle SprintStaminaTimerHandle;
    FTimerDelegate SprintStaminaTimerDelegate;

    // Trace interactable
    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float TraceInterval{0.1f};

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float TraceDistance{600.f};

    FTimerHandle TraceTimerHandle;
    IInteractable* FocusedInteractable{nullptr};

    UPROPERTY(EditAnywhere, Category = "TEST", meta = (AllowPrivateAccess = "true"))
    FItemSlot TestItemSlot;
};