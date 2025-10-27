// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/BaseCharacter.h"
#include "Animation/AnimationDataSubsystem.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimLayerInterface.h"
#include "Audio/DemoAudioSubsystem.h"
#include "Audio/DemoSoundTags.h"
#include "Character/DemoPawnData.h"
#include "Components/AttackCollisionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StateManagerComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/ActionInfoConfig.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/TableRowBases.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Items/Item.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    HitSoundTag = DemoSoundTags::SFX_Game_Hit_Default;
    MovementSpeedMode = DemoGameplayTags::Movement_SpeedMode_Jog;

    // @check - temporary values
    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    MovementComp->JumpZVelocity = 700.f;
    MovementComp->AirControl = 0.2f;
    MovementComp->MaxWalkSpeed = JogSpeed;
    MovementComp->GravityScale = 1.75f;
    MovementComp->MaxAcceleration = 1500.f;

    AttackCollisionComponent = CreateDefaultSubobject<UAttackCollisionComponent>(TEXT("AttackCollisionComponent"));

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

    EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

    StateManager = CreateDefaultSubobject<UStateManagerComponent>(TEXT("StateManager"));

    StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Validate
    if (!HitReactFrontMontage || !HitReactBackMontage || !HitParticle)
    {
        DemoLOG_CF(LogCharacter, Warning, TEXT("HitReact assets are not set for %s."), *GetName());
    }

    InitCharacter();
}

float ABaseCharacter::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Damage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

    // Defense
    const float Defense = StatsComponent->GetDerivedStatFinalValueChecked(DemoGameplayTags::Stat_Derived_Defense);
    Damage = Damage * (100.f / (100.f + Defense)); // Simple formula

    // @TODO
    // Deal with damage types

    // Block
    // @TODO - Wrap into a function?
    constexpr float BlockDamageMultiplier = 0.3f;
    constexpr float AllowBlockDegree = 45.f;
    if (bIsBlocking)
    {
        if (AActor* OtherActor = EventInstigator ? EventInstigator->GetPawn() : nullptr)
        {
            const FVector ToOther = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            const float FrontDot = FVector::DotProduct(GetActorForwardVector(), ToOther);

            if (FrontDot >= FMath::Cos(FMath::DegreesToRadians(AllowBlockDegree)))
            {
                // Successful block
                Damage *= BlockDamageMultiplier;
            }
        }
    }

    // Apply damage
    Damage = StatsComponent->TakeDamage(Damage);

    // Play hit effects
    PlayPointHitEffects(PointDamageEvent, EventInstigator);

    return Damage;
}

void ABaseCharacter::InitCharacter()
{
    if (!PawnData)
    {
        DemoLOG_CF(LogCharacter, Error, TEXT("PawnData is not set for %s."), *GetName());
        return;
    }

    // Init character identity
    CharacterTag = PawnData->CharacterTag;
    if (!CharacterTag.IsValid())
    {
        DemoLOG_CF(LogCharacter, Error, TEXT("CharacterTag is not set for %s."), *GetName());
    }
    CharacterDisplayName = PawnData->CharacterDisplayName;
    if (CharacterDisplayName.IsEmpty())
    {
        DemoLOG_CF(LogCharacter, Warning, TEXT("CharacterDisplayName is not set for %s."), *GetName());
        CharacterDisplayName = FText::FromString(GetName());
    }

    // Init StateManager
    StateManager->OnStateBegan.AddUObject(this, &ThisClass::HandleStateBegan);

    // Init StatsComponent
    for (const auto& [StatTag, ResourceStat] : PawnData->DefaultResourceStats)
    {
        StatsComponent->AddResourceStat(StatTag, ResourceStat);
    }
    for (const auto& [StatTag, PrimaryStat] : PawnData->DefaultPrimaryStats)
    {
        StatsComponent->AddPrimaryStat(StatTag, PrimaryStat);
    }
    for (const auto& [StatTag, DerivedStat] : PawnData->DefaultDerivedStats)
    {
        StatsComponent->AddDerivedStat(StatTag, DerivedStat);
    }
    for (const auto& [StatTag, PrimaryStat] : PawnData->DefaultPrimaryStats)
    {
        // @misc - Inefficient, but simple. Consider optimizing later.
        StatsComponent->RecalculateDerivedStat(StatTag);
    }
    StatsComponent->InitStatsComponent();
    StatsComponent->ResetAllResourceStats();
    StatsComponent->OnCurrentResourceStatChanged.AddUObject(this, &ThisClass::HandleCurrentResourceStatChanged);

    // Init EquipmentComponent
    EquipmentComponent->OnWeaponChanged.AddUObject(this, &ThisClass::HandleWeaponChanged);
    for (const FItemSlot& Slot : StartingItems)
    {
        EquipmentComponent->EquipItem(Slot);
    }
    const AItem* StartingWeapon = EquipmentComponent->GetEquippedItem(DemoGameplayTags::Item_Weapon);
    if (!StartingWeapon)
    {
        UpdateAnimationData(DemoGameplayTags::Item_Weapon_NoWeapon);
    }
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

    if (!StatsComponent->HasEnoughOrNoRestriction(DemoGameplayTags::Stat_Resource_Stamina, JumpStaminaCost))
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

void ABaseCharacter::SetBlockingState(bool bNewBlocking)
{
    if (bIsBlocking == bNewBlocking)
    {
        return;
    }
    bIsBlocking = bNewBlocking;

    OnBlockingStateChanged.ExecuteIfBound(bIsBlocking);
}

void ABaseCharacter::StartBlocking()
{
    bool bCanBlock = StateManager->CanPerformAction(DemoGameplayTags::State_Block);
    bCanBlock &= EquipmentComponent->GetEquippedItem(DemoGameplayTags::Item_Armor_Shield) != nullptr;

    if (bCanBlock)
    {
        SetBlockingState(true);
        StateManager->SetAction(DemoGameplayTags::State_Block);
    }
}

void ABaseCharacter::StopBlocking()
{
    SetBlockingState(false);

    if (StateManager->IsInState(DemoGameplayTags::State_Block))
    {
        StateManager->SetAction(DemoGameplayTags::State_General);
    }
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

void ABaseCharacter::PlayPointHitEffects(const FPointDamageEvent& PointDamageEvent, const AController* EventInstigator)
{
    AActor* OtherActor = EventInstigator ? EventInstigator->GetPawn() : nullptr;
    if (!OtherActor)
    {
        return;
    }

    const FVector ToOther = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    const float FrontDot = FVector::DotProduct(GetActorForwardVector(), ToOther);
    const float RightDot = FVector::DotProduct(GetActorRightVector(), ToOther);
    const FVector HitLocation = PointDamageEvent.HitInfo.ImpactPoint;

    // Sound
    if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
    {
        AudioSubsystem->PlaySoundAtLocation(this, HitSoundTag, HitLocation);
    }

    // Particle
    FTransform ParticleTransform;
    ParticleTransform.AddToTranslation(HitParticleTransform.GetTranslation() + HitLocation);
    ParticleTransform.SetRotation(PointDamageEvent.HitInfo.ImpactNormal.ToOrientationQuat() * HitParticleTransform.GetRotation());
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ParticleTransform);

    // Animation
    const float Cos45 = 1.f / FMath::Sqrt(2.f);
    if (!IsDead() && GetMesh())
    {
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            UAnimMontage* HitReactMontage = FrontDot >= 0.f ? HitReactFrontMontage : HitReactBackMontage;
            if (FMath::Abs(FrontDot) < Cos45) // Side
            {
                if (RightDot >= 0.f)
                {
                    HitReactMontage = HitReactRightMontage ? HitReactRightMontage.Get() : HitReactMontage;
                }
                else
                {
                    HitReactMontage = HitReactLeftMontage ? HitReactLeftMontage.Get() : HitReactMontage;
                }
            }

            const float Duration = AnimInstance->Montage_Play(HitReactMontage);
            if (Duration > 0.f)
            {
                // Cancel current action, but allow performing another action.
                StateManager->SetAction(DemoGameplayTags::State_General);
            }
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
    }
}

void ABaseCharacter::HandleDeath()
{
    DemoLOG_F(LogCharacter, Display, TEXT("%s has died."), *GetName());

    bIsDead = true;

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

    // Death sound
    if (DeathSoundTag.IsValid())
    {
        if (UDemoAudioSubsystem* AudioSubsystem = UGameInstance::GetSubsystem<UDemoAudioSubsystem>(GetGameInstance()))
        {
            AudioSubsystem->PlaySoundAtLocation(this, DeathSoundTag, GetActorLocation());
        }
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
    FTimerHandle DestroyTimerHandle;
    FTimerManager& TimerManager = GetWorldTimerManager();
    TimerManager.SetTimer(DestroyTimerHandle, this, &ThisClass::DestroyCharacter, DeathDestroyDelay);
}

void ABaseCharacter::HandleCurrentResourceStatChanged(FGameplayTag StatTag, float OldValue, float NewValue)
{
    if (StatTag == DemoGameplayTags::Stat_Resource_Health)
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

void ABaseCharacter::HandleWeaponChanged(const FWeaponData* WeaponData)
{
    const FGameplayTag NewWeaponTag = WeaponData ? WeaponData->ItemType : DemoGameplayTags::Item_Weapon_NoWeapon;
    UpdateAnimationData(NewWeaponTag);
}

void ABaseCharacter::UpdateAnimationData(FGameplayTag WeaponTag)
{
    UAnimationDataSubsystem* AnimationDataSubsystem = UGameInstance::GetSubsystem<UAnimationDataSubsystem>(GetGameInstance());
    if (!AnimationDataSubsystem)
    {
        DemoLOG_CF(LogCharacter, Error, TEXT("AnimationDataSubsystem not found."));
        CurrentActionInfo = nullptr;
        return;
    }

    CurrentActionInfo = AnimationDataSubsystem->GetActionInfoConfig(CharacterTag, WeaponTag);
    if (!CurrentActionInfo)
    {
        DemoLOG_CF(LogCharacter, Error, TEXT("Invalid ActionInfo for (%s, %s)."), *CharacterTag.ToString(), *WeaponTag.ToString());
    }

    if (GetMesh())
    {
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            if (AnimInstance->Implements<UAnimLayerInterface>())
            {
                if (TSubclassOf<UAnimInstance> ItemAnimLayerClass = AnimationDataSubsystem->GetItemAnimLayerClass(CharacterTag, WeaponTag))
                {
                    GetMesh()->LinkAnimClassLayers(ItemAnimLayerClass);
                }
                else
                {
                    DemoLOG_CF(LogCharacter, Error, TEXT("Invalid ItemAnimLayerClass for (%s, %s)."), *CharacterTag.ToString(), *WeaponTag.ToString());
                }
            }
        }
    }
}

bool ABaseCharacter::IsInAction(FGameplayTag Action) const
{
    return StateManager->IsInAction(Action);
}

bool ABaseCharacter::CanReceiveDamageFrom(const AActor* Attacker) const
{
    return Attacker && !IsDead() && !bIFrameEnabled;
}

float ABaseCharacter::CalculateDamage(EAttackCollisionType InType) const
{
    float Damage = 0.f;

    // Get base damage
    Damage = StatsComponent->GetDerivedStatFinalValueChecked(DemoGameplayTags::Stat_Derived_Attack);

    // Damage multiplier from action
    const FGameplayTag CurrentAction = StateManager->GetCurrentAction();
    if (const TArray<FActionInfo>* ActionInfosPtr = CurrentActionInfo->GetActionInfoArray(CurrentAction))
    {
        int32 Index = CombatComponent->GetCurrentActionIndex();
        if (ActionInfosPtr->IsValidIndex(Index))
        {
            Damage *= (*ActionInfosPtr)[Index].DamageMultiplier;
        }
    }

    // @TODO - Damage multiplier from stats, buffs, etc.

    return Damage;
}

int32 ABaseCharacter::GetActionInfoCount(FGameplayTag InAction) const
{
    if (CurrentActionInfo)
    {
        if (const TArray<FActionInfo>* ActionInfoArray = CurrentActionInfo->GetActionInfoArray(InAction))
        {
            return ActionInfoArray->Num();
        }
    }
    return 0;
}

FRotator ABaseCharacter::GetDesiredControlRotation() const
{
    FRotator Result = FRotator::ZeroRotator;
    Result.Yaw = GetControlRotation().Yaw;
    return Result;
}

FRotator ABaseCharacter::GetDesiredInputRotation() const
{
    // @check - Some enemies may want to do sidestep or backstep.
    return GetActorRotation();
}

float ABaseCharacter::PerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex)
{
    const FActionInfo* ActionInfo = PerformAction_Validate(InAction, bIgnoreCurrentState, MontageIndex, bUseRandomIndex);
    if (!ActionInfo)
    {
        return 0.f;
    }

    // Play montage
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    float Duration = AnimInstance->Montage_Play(ActionInfo->AnimMontage, ActionInfo->PlayRate, EMontagePlayReturnType::Duration);
    if (Duration == 0.f)
    {
        DemoLOG_CF(LogCharacter, Error, TEXT("Failed to play montage for %s."), *InAction.ToString());
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

const FActionInfo* ABaseCharacter::PerformAction_Validate(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex) const
{
    USkeletalMeshComponent* CharacterMesh = GetMesh();
    if (!CharacterMesh || !CharacterMesh->GetAnimInstance())
    {
        return nullptr;
    }

    if (!CurrentActionInfo)
    {
        return nullptr;
    }

    // Allowed in current state?
    if (!bIgnoreCurrentState && !StateManager->CanPerformAction(InAction))
    {
        return nullptr;
    }

    // Get action info
    const TArray<FActionInfo>* ActionInfoArray = CurrentActionInfo->GetActionInfoArray(InAction);
    if (!ActionInfoArray)
    {
        DemoLOG_CF(LogCharacter, Error, TEXT("Invalid ActionInfo for action %s."), *InAction.ToString());
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
        DemoLOG_CF(LogCharacter, Error, TEXT("Invalid index %d for action %s."), MontageIndex, *InAction.ToString());
        return nullptr;
    }

    // Check stamina
    const FActionInfo& ActionInfo = (*ActionInfoArray)[MontageIndex];
    if (!StatsComponent->HasEnoughOrNoRestriction(DemoGameplayTags::Stat_Resource_Stamina, ActionInfo.StaminaCost))
    {
        return nullptr;
    }

    // Check montage
    if (!ActionInfo.AnimMontage)
    {
        DemoLOG_CF(LogCharacter, Error, TEXT("Invalid AnimMontage for action %s."), *InAction.ToString());
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

void ABaseCharacter::SetOrientRotationToMovement(bool bOrient)
{
    GetCharacterMovement()->bUseControllerDesiredRotation = !bOrient;
    GetCharacterMovement()->bOrientRotationToMovement = bOrient;
}