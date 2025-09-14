// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/StateManagerComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    MovementSpeedMode = DemoGameplayTags::Movement_SpeedMode_Jog;

    // @check - temporary values
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    check(MovementComp);
    MovementComp->JumpZVelocity = 700.f;
    MovementComp->AirControl = 0.2f;
    MovementComp->MaxWalkSpeed = JogSpeed;
    MovementComp->GravityScale = 1.75f;
    MovementComp->MaxAcceleration = 1500.f;

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

    EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

    StateManager = CreateDefaultSubobject<UStateManagerComponent>(TEXT("StateManager"));

    StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
    // @TODO - Use data table or config file
    StatsComponent->AddResourceStat(UStatsComponent::HealthTag, FResourceStat{100.f, 100.f});
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    StateManager->OnStateBegan.AddUObject(this, &ThisClass::HandleStateBegan);

    StatsComponent->InitializeResourceStats();
    StatsComponent->OnCurrentResourceStatChanged.AddUObject(this, &ThisClass::OnCurrentResourceStatChanged);
}

bool ABaseCharacter::CanPerformJump() const
{
    if (!CanJump())
    {
        return false;
    }

    if (!StateManager->CanPerformAction(DemoGameplayTags::State_Jump))
    {
        return false;
    }

    return true;
}

void ABaseCharacter::Jump()
{
    if (!CanPerformJump())
    {
        return;
    }

    StateManager->SetAction(DemoGameplayTags::State_Jump);
    ACharacter::Jump();
}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
    StateManager->SetAction(DemoGameplayTags::State_General);
    Super::Landed(Hit);
}

void ABaseCharacter::OnCurrentResourceStatChanged(FGameplayTag StatTag, float OldValue, float NewValue)
{
    if (StatTag == UStatsComponent::HealthTag)
    {
        if (NewValue <= 0.f)
        {
            // @TODO - Death logic
        }
    }
}

void ABaseCharacter::HandleDeath()
{
    // @TODO
    // Stop animations to prevent notifies

    // Stop movement

    // Disable collisions

    // Animation of ragdoll

    // Drop weapon if any

    // Destroy actor after some time
}

void ABaseCharacter::HandleStateBegan(FGameplayTag NewState)
{
    if (NewState == DemoGameplayTags::State_Dead)
    {
        HandleDeath();
    }
}

void ABaseCharacter::SetMovementSpeedMode(FGameplayTag NewSpeedMode)
{
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (MovementSpeedMode == NewSpeedMode || !MovementComponent)
    {
        return;
    }

    MovementSpeedMode = NewSpeedMode;

    // Update speed
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
