// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    MovementSpeedMode = DemoGameplayTags::Movement_SpeedMode_Jog;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

    // TODO: temporary values
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.2f;
    GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
    GetCharacterMovement()->GravityScale = 1.75f;
    GetCharacterMovement()->MaxAcceleration = 1500.f;

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

    StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));

    // TODO: Use data table or config file
    StatsComponent->AddResourceStat(UStatsComponent::HealthTag, FResourceStat{100.f, 100.f, true});
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    StatsComponent->InitializeResourceStats();
    StatsComponent->OnCurrentResourceStatChanged.AddDynamic(this, &ThisClass::OnCurrentResourceStatChanged);
}

void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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
