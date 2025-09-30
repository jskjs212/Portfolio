// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/AttackTypes.h"
#include "CollisionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCollisionComponent, Log, All);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UCollisionComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UCollisionComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    ////////////////////////////////////////////////////////
    //        Collision functions
    ////////////////////////////////////////////////////////
public:
    // Add a new definition at runtime. Overwrite if the same type already exists.
    // i.e. equip weapon, enter new phase, etc.
    void AddAttackCollisionDefinition(const FAttackCollisionDefinition& InDefinition);

    // Remove a definition at runtime.
    // i.e. unequip weapon, lose body part, etc.
    void RemoveAttackCollisionDefinition(EAttackCollisionType InType);

    // Activate or deactivate a certain collision type.
    void SetAttackCollisionEnabled(EAttackCollisionType InType, bool bEnabled);

private:
    const FAttackCollisionDefinition* GetAttackCollisionDefinition(EAttackCollisionType InType) const;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    // Current active collision definitions.
    // Doesn't need TSet, usually 0~3 active unless the owner is a dragon?
    TArray<const FAttackCollisionDefinition*> ActiveCollisions;

    /* Attack collision */
    // All collision definitions. Not modified at runtime.
    // @TODO - DataAsset?
    UPROPERTY(EditAnywhere, Category = "Initialization")
    TArray<FAttackCollisionDefinition> AttackCollisionDefinitions;
};