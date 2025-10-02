// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/AttackTypes.h"
#include "CollisionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCollisionComponent, Log, All);

namespace EDrawDebugTrace { enum Type : int; }
struct FWeaponData;
class UEquipmentComponent;

/**
 * Collision component that handles attack collisions.
 * Non-weapon AttackCollisionDefinitions should be configured in the component.
 * Activate or deactivate with SetAttackCollisionEnabled().
 * Process active collisions in TickComponent().
 * Trace only for pawns for now.
 *
 * @Dependency - Some AttackCollisionTypes need EquipmentComponent.
 */
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
    // Process each active collision definition.
    void ProcessCollisionDefinition(const FAttackCollisionDefinition* InDefinition);

    void ProcessHit(const FHitResult& HitResult, const FAttackCollisionDefinition* InDefinition);

    void HandleWeaponChanged(const FWeaponData* WeaponData);

    const FAttackCollisionDefinition* GetAttackCollisionDefinition(EAttackCollisionType InType) const;

    // Get the world location of a socket depending on the collision type.
    // Following the rules in "DemoTypes/AttackTypes.h"
    FVector GetSocketLocation(EAttackCollisionType InType, FName InSocketName);

    const UEquipmentComponent* GetEquipmentComponent();

    ////////////////////////////////////////////////////////
    //        Variables - collision
    ////////////////////////////////////////////////////////
private:
    // Current active collision definitions.
    // Doesn't need TSet, usually 0~3 active unless the owner is a dragon?
    TArray<const FAttackCollisionDefinition*> ActiveCollisions;

    /* Attack collision */
    // All collision definitions.
    // @TODO - DataAsset?
    UPROPERTY(EditAnywhere, Category = "Initialization|Collision")
    TArray<FAttackCollisionDefinition> AttackCollisionDefinitions;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    static inline const TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes{UEngineTypes::ConvertToObjectType(ECC_Pawn)};

    UPROPERTY(EditAnywhere, Category = "Initialization|Collision")
    float DrawDebugDuration{3.f};

    UPROPERTY(EditAnywhere, Category = "Initialization|Collision")
    TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType{};

    TArray<AActor*> ActorsToIgnore;

    // @TODO - Can't handle multiple active collisions.
    TSet<AActor*> HitActors;

    TWeakObjectPtr<const UEquipmentComponent> CachedEquipmentComponent;
};