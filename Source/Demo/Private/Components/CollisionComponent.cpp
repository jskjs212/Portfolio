// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CollisionComponent.h"
#include "Components/EquipmentComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "Items/Item.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogCollisionComponent);

UCollisionComponent::UCollisionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCollisionComponent::BeginPlay()
{
    Super::BeginPlay();

    ActorsToIgnore.Add(GetOwner());

    // Bind to EquipmentComponent
    if (AActor* OwnerActor = GetOwner())
    {
        if (UEquipmentComponent* EquipmentComponent = OwnerActor->FindComponentByClass<UEquipmentComponent>())
        {
            CachedEquipmentComponent = EquipmentComponent;
            EquipmentComponent->OnWeaponChanged.AddUObject(this, &ThisClass::HandleWeaponChanged);
        }
        else
        {
            UE_LOG(LogCollisionComponent, Warning, TEXT("%s can't attack with weapon without EquipmentComponent."), *GetNameSafe(OwnerActor));
        }
    }

    for (const FAttackCollisionDefinition& Definition : AttackCollisionDefinitions)
    {
        // @TODO - Validate CollisionDefinitions: Valid sockets, types, etc.
    }
}

void UCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    for (const FAttackCollisionDefinition* Definition : ActiveCollisions)
    {
        ProcessCollisionDefinition(Definition);
    }
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
            SetAttackCollisionEnabled(InType, false);
            AttackCollisionDefinitions.RemoveAt(Index);
            return;
        }
    }
}

void UCollisionComponent::SetAttackCollisionEnabled(EAttackCollisionType InType, bool bEnabled)
{
    if (bEnabled)
    {
        const FAttackCollisionDefinition* Definition = GetAttackCollisionDefinition(InType);
        if (!Definition)
        {
            // Maybe anim triggered a type that has been removed by body part loss or smth.
            UE_LOG(LogCollisionComponent, Error, TEXT("UCollisionComponent::SetAttackCollisionEnabled - Invalid EAttackCollisionType for (%s, %s)."), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(InType));
            return;
        }

        if (!ActiveCollisions.Contains(Definition))
        {
            ActiveCollisions.Add(Definition);
            HitActors.Empty(); // @TODO
        }
        // @TEST
        UE_LOG(LogCollisionComponent, Warning, TEXT("Enabled collision for type %d."), static_cast<int32>(InType));
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

void UCollisionComponent::ProcessCollisionDefinition(const FAttackCollisionDefinition* InDefinition)
{
    for (const FAttackCollisionSegment& Segment : InDefinition->Segments)
    {
        const FVector StartLocation = GetSocketLocation(InDefinition->CollisionType, Segment.StartSocketName);
        const FVector EndLocation = GetSocketLocation(InDefinition->CollisionType, Segment.EndSocketName);

        // @TODO - Multiple times per frame?
        TArray<FHitResult> HitResults;
        bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
            this, /* WorldContextObject */
            StartLocation,
            EndLocation,
            Segment.Radius,
            TraceObjectTypes,
            false, /* bTraceComplex */
            ActorsToIgnore,
            DrawDebugType,
            HitResults,
            false, /* bIgnoreSelf, already in ActorsToIgnore */
            FLinearColor::Red,
            FLinearColor::Green,
            DrawDebugDuration
        );

        for (const FHitResult& HitResult : HitResults)
        {
            AActor* HitActor = HitResult.GetActor();
            if (!HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);
                ProcessHit(HitResult, InDefinition);
            }
        }
    }
}

void UCollisionComponent::ProcessHit(const FHitResult& HitResult, const FAttackCollisionDefinition* InDefinition)
{
    AActor* OwnerActor = GetOwner();
    ICombatInterface* OwnerCombatInterface = Cast<ICombatInterface>(OwnerActor);
    ICombatInterface* HitCombatInterface = Cast<ICombatInterface>(HitResult.GetActor());
    if (!OwnerActor || !OwnerCombatInterface || !HitCombatInterface || !HitCombatInterface->CanReceiveDamage())
    {
        // Only combat interface actors for now.
        return;
    }

    // @TODO - check team

    float Damage = OwnerCombatInterface->GetDamage(InDefinition->CollisionType);

    UGameplayStatics::ApplyPointDamage(
        HitResult.GetActor(),
        Damage,
        HitResult.ImpactNormal, /* HitFromDirection */
        HitResult,
        OwnerActor->GetInstigatorController(),
        OwnerActor,
        InDefinition->DamageType
    );
}

void UCollisionComponent::HandleWeaponChanged(const FWeaponData* WeaponData)
{
    if (WeaponData) // Equipped
    {
        AddAttackCollisionDefinition(WeaponData->AttackCollisionDefinition);
    }
    else // Unequipped
    {
        RemoveAttackCollisionDefinition(EAttackCollisionType::MainWeapon);
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

FVector UCollisionComponent::GetSocketLocation(EAttackCollisionType InType, FName InSocketName)
{
    if (InType == EAttackCollisionType::MainWeapon)
    {
        if (const UEquipmentComponent* EquipmentComponent = GetEquipmentComponent())
        {
            if (const AItem* MainWeapon = EquipmentComponent->GetEquippedItem(DemoGameplayTags::Item_Weapon))
            {
                if (const UMeshComponent* WeaponMesh = MainWeapon->GetMesh())
                {
                    return WeaponMesh->GetSocketLocation(InSocketName);
                }
            }
        }
    }
    else // Not MainWeapon
    {
        if (const ACharacter* OwnerCharacter = GetOwner<ACharacter>())
        {
            if (const USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh())
            {
                return CharacterMesh->GetSocketLocation(InSocketName);
            }
        }
    }

    UE_LOG(LogCollisionComponent, Error, TEXT("GetSocketLocation() - Failed to get socket location for type %s and socket %s."), *UEnum::GetValueAsString(InType), *InSocketName.ToString());
    return FVector::ZeroVector;
}

const UEquipmentComponent* UCollisionComponent::GetEquipmentComponent()
{
    if (!CachedEquipmentComponent.IsValid())
    {
        if (AActor* OwnerActor = GetOwner())
        {
            CachedEquipmentComponent = OwnerActor->FindComponentByClass<UEquipmentComponent>();
        }
    }
    return CachedEquipmentComponent.Get();
}