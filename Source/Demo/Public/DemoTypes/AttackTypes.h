// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackTypes.generated.h"

class UDamageType;

UENUM(BlueprintType)
enum class EAttackCollisionType : uint8
{
    None UMETA(Hidden),
    MainWeapon UMETA(DisplayName = "Main Weapon"),
    ShieldWeapon UMETA(DisplayName = "Shield Weapon"),
    RightFist UMETA(DisplayName = "RightFist"),
    LeftFist UMETA(DisplayName = "LeftFist"),
    Kick UMETA(DisplayName = "Kick"),
    Tail UMETA(DisplayName = "Tail")
};

USTRUCT()
struct FAttackCollisionSegment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
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
    float CollisionRadius{30.f};

    UPROPERTY(EditAnywhere)
    TArray<FAttackCollisionSegment> Segments;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UDamageType> DamageType;
};