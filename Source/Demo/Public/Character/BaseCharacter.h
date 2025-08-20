// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "BaseCharacter.generated.h"

class UCombatComponent;
class UStatsComponent;

UCLASS()
class DEMO_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(VisibleAnywhere, Category = "Character")
    TObjectPtr<UCombatComponent> CombatComponent;

    UPROPERTY(VisibleAnywhere, Category = "Character")
    TObjectPtr<UStatsComponent> StatsComponent;

    ////////////////////////////////////////////////////////
    //        Fundametals
    ////////////////////////////////////////////////////////
public:
    ABaseCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    ////////////////////////////////////////////////////////
    //        Character functions
    ////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
    UFUNCTION(BlueprintCallable, Category = "Character")
    void SetMovementSpeedMode(FGameplayTag NewSpeedMode);

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    /* Movement */
    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed = 180.f;
    UPROPERTY(EditAnywhere, Category = "Movement")
    float JogSpeed = 450.f;
    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeed = 800.f;

    FGameplayTag MovementSpeedMode;
};