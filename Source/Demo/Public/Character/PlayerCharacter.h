// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "DemoTypes/DemoTypes.h"
#include "DemoTypes/ItemTypes.h" // @TEST
#include "GenericTeamAgentInterface.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
struct FInputChord;
struct FWeaponData;
class IInteractable;
class UCameraComponent;
class UCurveFloat;
class UInputAction;
class UInputMappingContext;
class UInventoryComponent;
class USpringArmComponent;
class UTargetingComponent;

DECLARE_DELEGATE_OneParam(FOnInteractableFocused, IInteractable* /* NewFocusedInteractable */);

/**
 * Player character
 */
UCLASS()
class DEMO_API APlayerCharacter : public ABaseCharacter, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
private:
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

public:
    // PlayerCharacter ticks only after death to play timeline.
    virtual void Tick(float DeltaTime) override;

    ////////////////////////////////////////////////////////
    //        Character functions
    ////////////////////////////////////////////////////////
protected:
    virtual void HandleDeath() override;

    /* Event handlers */
    virtual void HandleWeaponChanged(const FWeaponData* WeaponData) override;

    ////////////////////////////////////////////////////////
    //        PlayerCharacter functions
    ////////////////////////////////////////////////////////
protected:
    // Load user settings and bind to changes.
    void LoadUserSettings();

    void ConsumeSprintStamina();

    // Trace first visible interactable from camera.
    IInteractable* TraceForInteractables();

    // Trace and broadcast if focused interactable changed.
    void HandleInteractable();

    void HandleTargetUpdated(AActor* NewActor);

private:
    void BindDeathCameraBoomTimeline();

    ////////////////////////////////////////////////////////
    //        Handlers
    ////////////////////////////////////////////////////////
private:
    void HandleDeathCameraBoomTimelineUpdate(float Alpha);

    void HandleControlsBoolUserSettingChanged(FGameplayTag InTag, bool bNewValue);

    void HandleInputKeySettingChanged(FGameplayTag InTag, const FInputChord& NewChord);

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

    // @param bReapplyMappings If false, only update runtime mapping context (For LoadUserSettings())
    void RebindInputKey(FGameplayTag InInputTag, const FInputChord& NewChord, bool bReapplyMappings);

    bool IsInputKeyBound(const FInputChord& InChord) const;

protected:
    void Look(const FInputActionValue& Value);

    void Move(const FInputActionValue& Value);
    void MoveComplete();

    virtual void Jump() override;
    virtual void Landed(const FHitResult& Hit) override;

    void StartWalking();
    void StopWalking();

    void StartSprinting();
    void StopSprinting();

    // Select one of possible interactions in order:
    // - Interact with focused interactable, if any.
    void Interact();

    /* Combat */
    void LightAttack();

    void Dodge();

    void ToggleLockOn();

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
private:
    UPROPERTY(EditAnywhere, Category = "Initialization|Input")
    TObjectPtr<const UInputMappingContext> DefaultMappingContext;

    UPROPERTY(VisibleAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> RuntimeDefaultMappingContext;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    /* Movement */
    // true = toggle, false = hold (user settings)
    bool bWalkInputToggle{true};
    // true = toggle, false = hold (user settings)
    bool bSprintInputToggle{false};

    FVector2D CachedMoveInputAxis;

    // Sprint
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Movement")
    float SprintStaminaCostPerSecond{10.f};
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Movement")
    float SprintStaminaInterval{0.1f};

    FTimerHandle SprintStaminaTimerHandle;
    FTimerDelegate SprintStaminaTimerDelegate;

    /* Interaction */
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Interaction")
    float InteractTraceInterval{0.1f};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Interaction")
    float InteractTraceDistance{600.f};

    FTimerHandle InteractTraceTimerHandle;
    IInteractable* FocusedInteractable{nullptr};

    /* Death */
    // Used to animate camera boom length change on death.
    // Length is set to DeathDestroyDelay in code, assume that curve length is 1 second.
    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Death")
    TObjectPtr<UCurveFloat> DeathCameraBoomCurve;

    FTimeline DeathCameraBoomTimeline;
    FOnTimelineFloatStatic DeathCameraBoomTimelineDelegate;

    /* Else */
    FGenericTeamId TeamID{DemoTeamID::Player};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Camera")
    float DefaultCameraBoomLength{300.f};

    UPROPERTY(EditDefaultsOnly, Category = "Initialization|Camera")
    float DeathCameraBoomLength{700.f};

    /* @TEST */
    UPROPERTY(EditDefaultsOnly, Category = "TEST", meta = (AllowPrivateAccess = "true"))
    FItemSlot TestItemSlot;
};