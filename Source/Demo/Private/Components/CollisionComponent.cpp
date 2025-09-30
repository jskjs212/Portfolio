// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CollisionComponent.h"
#include "Interfaces/CombatInterface.h"

DEFINE_LOG_CATEGORY(LogCollisionComponent);

UCollisionComponent::UCollisionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCollisionComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AttackCollisionDefinitions.Num() == 0)
    {
        UE_LOG(LogCollisionComponent, Warning, TEXT("No CollisionDefinitions set in %s."), *GetNameSafe(GetOwner()));
    }

    // @TODO - Validate CollisionDefinitions: Valid sockets, types, etc.
}

void UCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    for (const FAttackCollisionDefinition* Definition : ActiveCollisions)
    {
        // @TODO - Trace, apply damage, etc.
        // @TEST
        UE_LOG(LogCollisionComponent, Warning, TEXT("Tracing collision radius %.1f"), Definition->Segments[0].Radius);
    }
}

const FAttackCollisionDefinition* UCollisionComponent::GetAttackCollisionDefinition(EAttackCollisionType InType) const
{
    for (const FAttackCollisionDefinition& Definition : AttackCollisionDefinitions)
    {
        if (Definition.CollisionType == InType)
        {
            return &Definition;
        }
    }
    return nullptr;
}

void UCollisionComponent::AddAttackCollisionDefinition(const FAttackCollisionDefinition& InDefinition)
{
    if (InDefinition.CollisionType == EAttackCollisionType::None)
    {
        UE_LOG(LogCollisionComponent, Error, TEXT("AddAttackCollisionDefinition() - None type."));
        return;
    }

    for (int32 Index = 0; Index < AttackCollisionDefinitions.Num(); ++Index)
    {
        if (AttackCollisionDefinitions[Index].CollisionType == InDefinition.CollisionType)
        {
            AttackCollisionDefinitions[Index] = InDefinition;
            UE_LOG(LogCollisionComponent, Display, TEXT("AddAttackCollisionDefinition() - Overwrote existing definition for type %s."), *UEnum::GetValueAsString(InDefinition.CollisionType));
            return;
        }
    }

    AttackCollisionDefinitions.Add(InDefinition);
}

void UCollisionComponent::RemoveAttackCollisionDefinition(EAttackCollisionType InType)
{
    for (int32 Index = 0; Index < AttackCollisionDefinitions.Num(); ++Index)
    {
        if (AttackCollisionDefinitions[Index].CollisionType == InType)
        {
            AttackCollisionDefinitions.RemoveAt(Index);
            SetAttackCollisionEnabled(InType, false);
            return;
        }
    }
}

void UCollisionComponent::SetAttackCollisionEnabled(EAttackCollisionType InType, bool bEnabled)
{
    if (bEnabled)
    {
        // @TODO
        const FAttackCollisionDefinition* Definition = GetAttackCollisionDefinition(InType);
        if (!Definition)
        {
            UE_LOG(LogCollisionComponent, Error, TEXT("UCollisionComponent::SetAttackCollisionEnabled - Invalid EAttackCollisionType for (%s, %s)."), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(InType));
            return;
        }

        if (!ActiveCollisions.Contains(Definition))
        {
            ActiveCollisions.Add(Definition);
            // @TEST
            UE_LOG(LogCollisionComponent, Warning, TEXT("Enabled collision for type %d."), static_cast<int32>(InType));
        }
    }
    else // Disable
    {
        for (int32 Index = 0; Index < ActiveCollisions.Num(); ++Index)
        {
            if (ActiveCollisions[Index]->CollisionType == InType)
            {
                ActiveCollisions.RemoveAt(Index);
                // @TEST
                UE_LOG(LogCollisionComponent, Warning, TEXT("Disabled collision for type %d."), static_cast<int32>(InType));
                return;
            }
        }
    }
}