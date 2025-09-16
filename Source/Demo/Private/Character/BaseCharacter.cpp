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

    EquipmentComponent->OnWeaponChanged.BindUObject(this, &ThisClass::UpdateActionInfo);

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

    if (!StatsComponent->HasEnoughOrNoRestriction(UStatsComponent::StaminaTag, JumpStaminaCost))
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
    Super::Jump();

    StatsComponent->ConsumeStamina(JumpStaminaCost);
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
    if (USkeletalMeshComponent* CharacterMesh = GetMesh())
    {
        CharacterMesh->SetCollisionProfileName(TEXT("Ragdoll"));
        CharacterMesh->SetAllBodiesSimulatePhysics(true);
        // @TEST
        //CharacterMesh->SetSimulatePhysics(true);
        //CharacterMesh->WakeAllRigidBodies();
        //CharacterMesh->bBlendPhysics = true;
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

float ABaseCharacter::PerformAction(FGameplayTag InAction, int32 MontageIndex, bool bUseRandomIndex)
{
    USkeletalMeshComponent* CharacterMesh = GetMesh();
    if (!CharacterMesh)
    {
        return 0.f;
    }

    UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
    if (!AnimInstance)
    {
        return 0.f;
    }

    const FActionInfo* ActionInfo = CanPerformAction(InAction, MontageIndex, bUseRandomIndex);
    if (!ActionInfo)
    {
        return 0.f;
    }

    // Play montage
    float Duration = AnimInstance->Montage_Play(ActionInfo->AnimMontage, ActionInfo->PlayRate, EMontagePlayReturnType::Duration);
    if (Duration == 0.f)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to play montage for action %s."), *InAction.ToString());
        return 0.f;
    }

    // Jump to section
    if (ActionInfo->StartSection != NAME_None)
    {
        AnimInstance->Montage_JumpToSection(ActionInfo->StartSection, ActionInfo->AnimMontage);
    }

    StateManager->SetAction(InAction);
    StatsComponent->ConsumeStamina(ActionInfo->StaminaCost);

    return Duration;
}

const FActionInfo* ABaseCharacter::CanPerformAction(FGameplayTag InAction, int32 MontageIndex, bool bUseRandomIndex) const
{
    if (!CurrentActionInfo)
    {
        UE_LOG(LogTemp, Error, TEXT("ABaseCharacter::CanPerformAction - CurrentActionInfo is null."));
        return nullptr;
    }

    // Allowed in current state?
    if (!StateManager->CanPerformAction(InAction))
    {
        return nullptr;
    }

    // Get action info
    const TArray<FActionInfo>* ActionInfoArray = CurrentActionInfo->GetActionInfoArray(InAction);
    if (!ActionInfoArray)
    {
        UE_LOG(LogTemp, Error, TEXT("ABaseCharacter::CanPerformAction - Invalid ActionInfo for action %s."), *InAction.ToString());
        return nullptr;
    }

    // Check montage index
    if (bUseRandomIndex)
    {
        // Random index
        MontageIndex = FMath::RandRange(0, ActionInfoArray->Num() - 1);
    }
    else if (MontageIndex < 0 || MontageIndex >= ActionInfoArray->Num())
    {
        UE_LOG(LogTemp, Error, TEXT("ABaseCharacter::CanPerformAction - Invalid index %d for action %s."), MontageIndex, *InAction.ToString());
        return nullptr;
    }

    // Check stamina
    const FActionInfo& ActionInfo = (*ActionInfoArray)[MontageIndex];
    if (!StatsComponent->HasEnoughOrNoRestriction(UStatsComponent::StaminaTag, ActionInfo.StaminaCost))
    {
        return nullptr;
    }

    // Check montage
    if (!ActionInfo.AnimMontage)
    {
        UE_LOG(LogTemp, Error, TEXT("ABaseCharacter::CanPerformAction - Invalid AnimMontage for action %s."), *InAction.ToString());
        return nullptr;
    }

    return &ActionInfo;
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
