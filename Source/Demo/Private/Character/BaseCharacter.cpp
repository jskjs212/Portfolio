// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Animation/ActionInfoSubsystem.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StateManagerComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/ActionInfoConfig.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"

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

    if (!IdentityTag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("IdentityTag is not set for %s."), *GetName());
    }

    UpdateActionInfo(DemoGameplayTags::Item_Weapon_NoWeapon);

    StateManager->OnStateBegan.AddUObject(this, &ThisClass::HandleStateBegan);

    StatsComponent->InitializeResourceStats();
    StatsComponent->OnCurrentResourceStatChanged.AddUObject(this, &ThisClass::HandleCurrentResourceStatChanged);
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

    if (!StatsComponent->HasEnough(UStatsComponent::StaminaTag, JumpStaminaCost))
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
    StateManager->OnLanded();
    Super::Landed(Hit);
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

    // Set Jump state when falling.
    // If already performing another action, then carry on.
    if (GetCharacterMovement()->IsFalling())
    {
        if (StateManager->CanPerformAction(DemoGameplayTags::State_Jump))
        {
            StateManager->SetAction(DemoGameplayTags::State_Jump);
        }
    }
}

void ABaseCharacter::DestroyCharacter()
{
    EquipmentComponent->DestroyAllEquippedItems();
    Destroy();
}

void ABaseCharacter::EnableRagdoll()
{
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
        MeshComp->SetAllBodiesSimulatePhysics(true);
        // @TEST
        //MeshComp->SetSimulatePhysics(true);
        //MeshComp->WakeAllRigidBodies();
        //MeshComp->bBlendPhysics = true;
    }
}

void ABaseCharacter::HandleDeath()
{
    // @TODO
    // Stop animations to prevent notifies
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        constexpr float BlendOutTime = 0.3f;
        AnimInstance->StopAllMontages(BlendOutTime);
    }

    // Stop movement
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->DisableMovement();
        //MovementComponent->StopMovementImmediately();
    }

    // Disable collisions
    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // Animation of ragdoll
    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
    }
    else
    {
        EnableRagdoll();
    }

    // Drop weapon if any
    AItem* Weapon = EquipmentComponent->GetEquippedItem(DemoGameplayTags::Item_Weapon);
    if (Weapon)
    {
        Weapon->SimulatePhysics();
    }

    // Stop timers
    StatsComponent->StopAllRegen();

    // Destroy actor after some time
    constexpr float DestroyDelay = 5.f;
    FTimerHandle DestroyTimerHandle;
    FTimerManager& TimerManager = GetWorldTimerManager();
    TimerManager.SetTimer(DestroyTimerHandle, this, &ThisClass::DestroyCharacter, DestroyDelay);
}

void ABaseCharacter::HandleCurrentResourceStatChanged(FGameplayTag StatTag, float OldValue, float NewValue)
{
    if (StatTag == UStatsComponent::HealthTag)
    {
        if (NewValue <= 0.f)
        {
            StateManager->SetAction(DemoGameplayTags::State_Dead);
        }
    }
}

void ABaseCharacter::HandleStateBegan(FGameplayTag NewState)
{
    if (NewState == DemoGameplayTags::State_Dead)
    {
        HandleDeath();
    }
}

void ABaseCharacter::UpdateActionInfo(FGameplayTag WeaponTag)
{
    UActionInfoSubsystem* ActionInfoSubsystem = UGameInstance::GetSubsystem<UActionInfoSubsystem>(GetGameInstance());
    if (!ActionInfoSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("ActionInfoSubsystem not found."));
        CurrentActionInfo = nullptr;
        return;
    }

    CurrentActionInfo = ActionInfoSubsystem->GetActionInfoConfig(IdentityTag, WeaponTag);

    if (CurrentActionInfo == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid ActionInfo for (%s, %s)."), *IdentityTag.ToString(), *WeaponTag.ToString());
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
