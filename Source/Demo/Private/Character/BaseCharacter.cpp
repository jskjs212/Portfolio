// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    MovementSpeedMode = DemoGameplayTags::Movement_SpeedMode_Jog;

    // check: temporary values
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    check(MovementComp);
    MovementComp->JumpZVelocity = 700.f;
    MovementComp->AirControl = 0.2f;
    MovementComp->MaxWalkSpeed = JogSpeed;
    MovementComp->GravityScale = 1.75f;
    MovementComp->MaxAcceleration = 1500.f;

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

    EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

    StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
    // TODO: Use data table or config file
    StatsComponent->AddResourceStat(UStatsComponent::HealthTag, FResourceStat{100.f, 100.f});
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    StatsComponent->InitializeResourceStats();
    StatsComponent->OnCurrentResourceStatChanged.AddUObject(this, &ThisClass::OnCurrentResourceStatChanged);
}

void ABaseCharacter::OnCurrentResourceStatChanged(FGameplayTag StatTag, float OldValue, float NewValue)
{
    if (StatTag == UStatsComponent::HealthTag)
    {
        if (NewValue <= 0.f)
        {
            // TODO: Death logic
        }
    }
}

void ABaseCharacter::SetMovementSpeedMode(FGameplayTag NewSpeedMode)
{
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (MovementSpeedMode == NewSpeedMode || !MovementComponent)
    {
        return;
    }

    // TODO: Stop consume stamina if sprinting

    MovementSpeedMode = NewSpeedMode;

    if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Walk)
    {
        MovementComponent->MaxWalkSpeed = WalkSpeed;
    }
    else if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Jog)
    {
        MovementComponent->MaxWalkSpeed = JogSpeed;
    }
    else if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Sprint)
    {
        MovementComponent->MaxWalkSpeed = SprintSpeed;
    }
}
