// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackTypes.generated.h"

class UDamageType;

// Types of attack collision.
// AnimNotifyState_CollisionTrace will enable a certain type during its duration.
// When a monster has multiple attack sources like 6 arms, they can be differentiated by type and enabled in their own attack timings.
// !!! AttackCollisionDefinition should be added to the CollisionComponent first for each type.
// !!! MainWeapon type will find sockets from the equipped weapon's mesh.
// !!! Other types will find sockets from the owner character's mesh.
// Not implemented examples: Shield (bash), SecondWeapon (dual wield), etc.
UENUM(BlueprintType)
enum class EAttackCollisionType : uint8
{
    None UMETA(Hidden),
    MainWeapon UMETA(DisplayName = "Main Weapon"),
    Head UMETA(DisplayName = "Head"),
    RightArm UMETA(DisplayName = "Right Arm"),
    LeftArm UMETA(DisplayName = "Left Arm"),
    ThirdArm UMETA(DisplayName = "Third Arm"),
    Leg UMETA(DisplayName = "Leg"),
    Tail UMETA(DisplayName = "Tail")
};

USTRUCT()
struct FAttackCollisionSegment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.001"))
    float Radius{30.f};

    UPROPERTY(EditAnywhere)
    FName StartSocketName;

    UPROPERTY(EditAnywhere)
    FName EndSocketName;
};

USTRUCT()
struct FAttackCollisionDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EAttackCollisionType CollisionType{EAttackCollisionType::None};

    UPROPERTY(EditAnywhere)
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditAnywhere)
    TArray<FAttackCollisionSegment> Segments;

    bool IsValid() const
    {
        return CollisionType != EAttackCollisionType::None && Segments.Num() > 0 && DamageType != nullptr;
    }
};