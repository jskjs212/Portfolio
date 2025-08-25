// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Items/ItemTypes.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInventoryComponent;
class USpringArmComponent;

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
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    APlayerCharacter();

    ////////////////////////////////////////////////////////
    //        Input functions
    ////////////////////////////////////////////////////////
public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    void Look(const FInputActionValue& Value);
    void Move(const FInputActionValue& Value);

    void StartWalk();
    void StopWalk();
    void StartSprint();
    void StopSprint();

    // TEST:
    UFUNCTION(BlueprintNativeEvent)
    void Test1();

    UFUNCTION(BlueprintNativeEvent)
    void Test2();

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

    // TEST:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> Test1Action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> Test2Action;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    // Options: true = toggle, false = hold
    bool bIsWalkInputTogglesWalk{true};
    bool bIsSprintInputTogglesSprint{false};

    // TEST:
    UPROPERTY(EditAnywhere, Category = "Test")
    FItemSlot TestWeapon;
    UPROPERTY(EditAnywhere, Category = "Test")
    FItemSlot TestShield;
};