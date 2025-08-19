// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UCameraComponent;
class UInputAction;
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
public:
    UPROPERTY(VisibleAnywhere, Category = "Character")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = "Character")
    TObjectPtr<UCameraComponent> FollowCamera;

    ////////////////////////////////////////////////////////
    //        Fundametals
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

    ////////////////////////////////////////////////////////
    //        Input variables
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;
};