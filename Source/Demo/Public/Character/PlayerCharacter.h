// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Items/ItemTypes.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class IInteractable;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInventoryComponent;
class USpringArmComponent;
class USoundBase;

DECLARE_DELEGATE_OneParam(FOnInteractableFocused, IInteractable* /* NewFocusedInteractable */);

/**
 *
 */
UCLASS()
class DEMO_API APlayerCharacter : public ABaseCharacter
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

    // Trace and broadcast if focused interactable changed.
    void HandleInteractable();

    ////////////////////////////////////////////////////////
    //        Input functions
    ////////////////////////////////////////////////////////
public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    void Look(const FInputActionValue& Value);
    void Move(const FInputActionValue& Value);

    virtual void Jump() override;
    virtual void Landed(const FHitResult& Hit) override;

    void StartWalk();
    void StopWalk();
    void StartSprint();
    void StopSprint();

    void Interact();

    void ShowPlayerMenu();

    // TEST:
    UFUNCTION(BlueprintNativeEvent)
    void Test1();

    UFUNCTION(BlueprintNativeEvent)
    void Test2();

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    // check: Call in AnimInstance?
    FORCEINLINE float GetGroundSpeed() const
    {
        FVector Velocity = GetVelocity();
        Velocity.Z = 0.f;
        return Velocity.Size();
    }

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

    // TEST:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> Test1Action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> Test2Action;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
public:
    // TODO: Audio
    UPROPERTY(EditDefaultsOnly, Category = "Item|Audio")
    TObjectPtr<USoundBase> PickupSound;

private:
    /* Movement */
    // Options: true = toggle, false = hold
    bool bIsWalkInputTogglesWalk{true};
    bool bIsSprintInputTogglesSprint{false};

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float JumpStaminaCost{10.f};

    // Sprint
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintStaminaCostPerSecond{10.f};
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float SprintStaminaInterval{0.1f};

    FTimerHandle SprintStaminaTimerHandle;
    FTimerDelegate SprintStaminaTimerDelegate;

    // Trace interactable
    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float TraceInterval{0.2f};

    UPROPERTY(EditDefaultsOnly, Category = "Trace")
    float TraceDistance{600.f};

    FTimerHandle TraceTimerHandle;
    IInteractable* FocusedInteractable{nullptr};

    // TEST:
    UPROPERTY(EditAnywhere, Category = "Test")
    FItemSlot TestWeapon;
    UPROPERTY(EditAnywhere, Category = "Test")
    FItemSlot TestShield;
    UPROPERTY(EditAnywhere, Category = "Test")
    FItemSlot TestFood;
};