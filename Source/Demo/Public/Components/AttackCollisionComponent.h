// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DemoTypes/AttackTypes.h"
#include "AttackCollisionComponent.generated.h"

namespace EDrawDebugTrace { enum Type : int; }
struct FWeaponData;
class AItem;
class UEquipmentComponent;

struct FActiveAttackCollisionDefinition
{
    const FAttackCollisionDefinition* Definition{nullptr};
    int32 HitGroup{-1};

    bool operator==(const FActiveAttackCollisionDefinition& Other) const
    {
        return Definition == Other.Definition && HitGroup == Other.HitGroup;
    }
};

/**
 * Component that handles attack collisions.
 * Non-weapon AttackCollisionDefinitions should be configured in the component.
 * Activate or deactivate with SetAttackCollisionEnabled().
 * Process active collisions in TickComponent().
 * Trace only for pawns for now.
 * !!! HitActors are not cleared on deactivation.
 * !!! Assume that bClearHitActorsOnBegin is true at least once for each AnimMontage.
 *
 * @Dependency - Some AttackCollisionTypes need EquipmentComponent.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UAttackCollisionComponent : public UActorComponent
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    UAttackCollisionComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    ////////////////////////////////////////////////////////
    //        Collision functions
    ////////////////////////////////////////////////////////
public:
    // Add a new definition at runtime. Overwrite if the same type already exists.
    // e.g. equip weapon, enter new phase, etc.
    void AddAttackCollisionDefinition(const FAttackCollisionDefinition& InDefinition);

    // Remove a definition at runtime.
    // e.g. unequip weapon, lose body part, etc.
    void RemoveAttackCollisionDefinition(EAttackCollisionType InType);

    void ActivateCollisionDefinition(EAttackCollisionType InType, int32 HitGroup, bool bClearHitActorsOnBegin);

    void DeactivateCollisionDefinition(EAttackCollisionType InType, int32 HitGroup);

    // Deactivate all hit groups of the type.
    void DeactivateCollisionDefinition(EAttackCollisionType InType);

private:
    // Process each active collision definition.
    void ProcessCollisionDefinition(const FActiveAttackCollisionDefinition& ActiveDefinition);

    void ProcessHit(const FHitResult& HitResult, EAttackCollisionType InType, TSubclassOf<UDamageType> InDamageType);

    void HandleWeaponChanged(const FWeaponData* WeaponData);

    const FAttackCollisionDefinition* GetAttackCollisionDefinition(EAttackCollisionType InType) const;

    // Get the world location of a socket depending on the collision type.
    // Following the rules in "DemoTypes/AttackTypes.h"
    FVector GetSocketLocation(EAttackCollisionType InType, FName InSocketName);

    const UEquipmentComponent* GetEquipmentComponent();
    AItem* GetMainWeapon();

    ////////////////////////////////////////////////////////
    //        Variables - collision
    ////////////////////////////////////////////////////////
private:
    // Current active attack collision definitions.
    TArray<FActiveAttackCollisionDefinition> ActiveDefinitions;

    /* Attack collision */
    // All collision definitions.
    // @TODO - DataAsset?
    UPROPERTY(EditAnywhere, Category = "Initialization|Collision")
    TArray<FAttackCollisionDefinition> AttackCollisionDefinitions;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    static const TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

    UPROPERTY(EditAnywhere, Category = "Initialization|Collision")
    float DrawDebugDuration{3.f};

    UPROPERTY(EditAnywhere, Category = "Initialization|Collision")
    TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType{};

    TArray<AActor*> ActorsToIgnore;

    TArray<TSet<AActor*>> HitActorGroups;

    TWeakObjectPtr<const UEquipmentComponent> CachedEquipmentComponent;
    TWeakObjectPtr<AItem> CachedMainWeapon;
};