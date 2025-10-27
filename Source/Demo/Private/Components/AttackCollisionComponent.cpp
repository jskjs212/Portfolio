// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/AttackCollisionComponent.h"
#include "Components/EquipmentComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "Items/Item.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Static variable initialization
const TArray<TEnumAsByte<EObjectTypeQuery>> UAttackCollisionComponent::TraceObjectTypes{UEngineTypes::ConvertToObjectType(ECC_Pawn)};

UAttackCollisionComponent::UAttackCollisionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    DrawDebugType = EDrawDebugTrace::None;
}

void UAttackCollisionComponent::BeginPlay()
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
            DemoLOG_CF(LogCombat, Warning, TEXT("%s can't attack with weapon without EquipmentComponent."), *GetNameSafe(OwnerActor));
        }
    }

    for (const FAttackCollisionDefinition& Definition : AttackCollisionDefinitions)
    {
        // @TODO - Validate CollisionDefinitions: Valid sockets, types, etc.
    }
}

void UAttackCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    for (const FActiveAttackCollisionDefinition& ActiveDefinition : ActiveDefinitions)
    {
        ProcessCollisionDefinition(ActiveDefinition);
    }
}

void UAttackCollisionComponent::AddAttackCollisionDefinition(const FAttackCollisionDefinition& InDefinition)
{
    if (InDefinition.CollisionType == EAttackCollisionType::None)
    {
        DemoLOG_CF(LogCombat, Error, TEXT("None type."));
        return;
    }

    for (int32 Index = 0; Index < AttackCollisionDefinitions.Num(); ++Index)
    {
        if (AttackCollisionDefinitions[Index].CollisionType == InDefinition.CollisionType)
        {
            AttackCollisionDefinitions[Index] = InDefinition;
            DemoLOG_CF(LogCombat, Display, TEXT("Overwrote existing definition for type %s."), *UEnum::GetValueAsString(InDefinition.CollisionType));
            return;
        }
    }

    AttackCollisionDefinitions.Add(InDefinition);
}

void UAttackCollisionComponent::RemoveAttackCollisionDefinition(EAttackCollisionType InType)
{
    for (int32 Index = 0; Index < AttackCollisionDefinitions.Num(); ++Index)
    {
        if (AttackCollisionDefinitions[Index].CollisionType == InType)
        {
            DeactivateCollisionDefinition(InType);
            AttackCollisionDefinitions.RemoveAt(Index);
            return;
        }
    }
}

void UAttackCollisionComponent::ActivateCollisionDefinition(EAttackCollisionType InType, int32 HitGroup, bool bClearHitActorsOnBegin)
{
    checkf(HitGroup >= 0, TEXT("Owner: %s, Type: %s"), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(InType)); // unlikely

    const FAttackCollisionDefinition* Definition = GetAttackCollisionDefinition(InType);
    if (!Definition)
    {
        // Maybe anim triggered a type that has been removed by body part loss or smth.
        DemoLOG_CF(LogCombat, Warning, TEXT("Invalid EAttackCollisionType for (%s, %s)."), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(InType));
        return;
    }

    // Add hit actor group if necessary.
    if (HitGroup >= HitActorGroups.Num())
    {
        HitActorGroups.SetNum(HitGroup + 1);
    }

    // Activate
    auto ActiveDefinition = FActiveAttackCollisionDefinition{Definition, HitGroup};
    if (!ActiveDefinitions.Contains(ActiveDefinition))
    {
        ActiveDefinitions.Add(ActiveDefinition);

        if (bClearHitActorsOnBegin)
        {
            HitActorGroups[HitGroup].Empty();
        }
    }

    // @debug
    UE_LOG(LogCombat, Verbose, TEXT("Enabled collision - Type: %s, HitGroup: %d"), *UEnum::GetValueAsString(InType), HitGroup);
}

void UAttackCollisionComponent::DeactivateCollisionDefinition(EAttackCollisionType InType, int32 HitGroup)
{
    for (int32 Index = 0; Index < ActiveDefinitions.Num(); ++Index)
    {
        if (ActiveDefinitions[Index].HitGroup == HitGroup && ActiveDefinitions[Index].Definition->CollisionType == InType)
        {
            ActiveDefinitions.RemoveAt(Index);
            // @debug
            UE_LOG(LogCombat, Verbose, TEXT("Disabled collision - Type: %s, HitGroup: %d"), *UEnum::GetValueAsString(InType), HitGroup);
            return;
        }
    }
}

void UAttackCollisionComponent::DeactivateCollisionDefinition(EAttackCollisionType InType)
{
    for (int32 Index = ActiveDefinitions.Num() - 1; Index >= 0; --Index)
    {
        if (ActiveDefinitions[Index].Definition->CollisionType == InType)
        {
            ActiveDefinitions.RemoveAt(Index);
        }
    }
}

void UAttackCollisionComponent::ProcessCollisionDefinition(const FActiveAttackCollisionDefinition& ActiveDefinition)
{
    // Trace each segment.
    const FAttackCollisionDefinition* Definition = ActiveDefinition.Definition;
    if (Definition->CollisionType == EAttackCollisionType::MainWeapon && !GetMainWeapon())
    {
        return;
    }

    for (const FAttackCollisionSegment& Segment : Definition->Segments)
    {
        const FVector StartLocation = GetSocketLocation(Definition->CollisionType, Segment.StartSocketName);
        const FVector EndLocation = Segment.StartSocketName == Segment.EndSocketName ? StartLocation : GetSocketLocation(Definition->CollisionType, Segment.EndSocketName);

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

        TSet<AActor*>& HitActors = HitActorGroups[ActiveDefinition.HitGroup];
        for (const FHitResult& HitResult : HitResults)
        {
            AActor* HitActor = HitResult.GetActor();
            if (!HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);
                ProcessHit(HitResult, Definition->CollisionType, Definition->DamageType);
            }
        }
    }
}

void UAttackCollisionComponent::ProcessHit(const FHitResult& HitResult, EAttackCollisionType InType, TSubclassOf<UDamageType> InDamageType)
{
    AActor* OwnerActor = GetOwner();
    ICombatInterface* OwnerCombatInterface = Cast<ICombatInterface>(OwnerActor);
    ICombatInterface* HitCombatInterface = Cast<ICombatInterface>(HitResult.GetActor());
    if (!OwnerActor || !OwnerCombatInterface || !HitCombatInterface)
    {
        // Only combat interface actors for now.
        return;
    }

    if (!HitCombatInterface->CanReceiveDamageFrom(OwnerActor))
    {
        // Dead, friendly, etc.
        return;
    }

    float Damage = OwnerCombatInterface->CalculateDamage(InType);
    if (Damage <= 0.f)
    {
        return;
    }

    // Apply damage
    AController* Instigator = OwnerActor->GetInstigatorController();
    AActor* DamageCauser = InType == EAttackCollisionType::MainWeapon ? GetMainWeapon() : OwnerActor;
    UGameplayStatics::ApplyPointDamage(
        HitResult.GetActor(),
        Damage,
        HitResult.ImpactNormal, /* HitFromDirection */
        HitResult,
        Instigator,
        DamageCauser,
        InDamageType
    );
}

void UAttackCollisionComponent::HandleWeaponChanged(const FWeaponData* WeaponData)
{
    CachedMainWeapon = nullptr;

    if (WeaponData) // Equipped
    {
        GetMainWeapon(); // Update CachedMainWeapon
        AddAttackCollisionDefinition(WeaponData->AttackCollisionDefinition);
    }
    else // Unequipped
    {
        RemoveAttackCollisionDefinition(EAttackCollisionType::MainWeapon);
    }
}

const FAttackCollisionDefinition* UAttackCollisionComponent::GetAttackCollisionDefinition(EAttackCollisionType InType) const
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

FVector UAttackCollisionComponent::GetSocketLocation(EAttackCollisionType InType, FName InSocketName)
{
    if (InType == EAttackCollisionType::MainWeapon)
    {
        if (const AItem* MainWeapon = GetMainWeapon())
        {
            if (const UMeshComponent* WeaponMesh = MainWeapon->GetMesh())
            {
                return WeaponMesh->GetSocketLocation(InSocketName);
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

    DemoLOG_CF(LogCombat, Error, TEXT("Failed to get socket location for type %s and socket %s."), *UEnum::GetValueAsString(InType), *InSocketName.ToString());
    return FVector::ZeroVector;
}

const UEquipmentComponent* UAttackCollisionComponent::GetEquipmentComponent()
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

AItem* UAttackCollisionComponent::GetMainWeapon()
{
    if (!CachedMainWeapon.IsValid())
    {
        if (const UEquipmentComponent* EquipmentComponent = GetEquipmentComponent())
        {
            CachedMainWeapon = EquipmentComponent->GetEquippedItem(DemoGameplayTags::Item_Weapon);
        }
    }
    return CachedMainWeapon.Get();
}