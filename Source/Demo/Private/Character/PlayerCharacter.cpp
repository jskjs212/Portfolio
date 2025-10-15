// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StateManagerComponent.h"
#include "Components/StatsComponent.h"
#include "Components/TargetingComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "DemoTypes/TableRowBases.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Interfaces/Interactable.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerController/DemoPlayerController.h"

APlayerCharacter::APlayerCharacter() :
    SprintStaminaTimerDelegate{FTimerDelegate::CreateUObject(this, &ThisClass::ConsumeSprintStamina)}
{
    // Disable character rotation by controller
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    CapsuleComp->InitCapsuleSize(42.f, 96.f);

    // Adjust for Elf_Arden mesh
    USkeletalMeshComponent* MeshComp = GetMesh();
    MeshComp->SetRelativeLocation(FVector{0.f, 0.f, -96.f});
    MeshComp->SetRelativeRotation(FRotator{0.f, -90.f, 0.f});

    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    MovementComp->bUseControllerDesiredRotation = false;
    MovementComp->bOrientRotationToMovement = true;
    MovementComp->RotationRate = FRotator{0.f, 540.f, 0.f};

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Camera position
    CameraBoom->SetRelativeLocation(FVector{0.f, 0.f, 160.f});
    CameraBoom->SocketOffset = FVector{0.f, 50.f, 0.f};

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

    TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    TargetingComponent->OnTargetUpdated.AddUObject(this, &ThisClass::HandleTargetUpdated);

    // Init UI
    if (ADemoPlayerController* DemoPlayerController = GetController<ADemoPlayerController>())
    {
        DemoPlayerController->InitUI();
    }

    // Set timers for interactable tracing and sprint stamina consumption.
    // Trace for interactables at intervals rather than every tick.
    if (UWorld* World = GetWorld())
    {
        FTimerManager& TimerManager = World->GetTimerManager();
        TimerManager.SetTimer(TraceTimerHandle, this, &ThisClass::HandleInteractable, TraceInterval, true);
        TimerManager.SetTimer(SprintStaminaTimerHandle, SprintStaminaTimerDelegate, SprintStaminaInterval, true);
        TimerManager.PauseTimer(SprintStaminaTimerHandle);
    }
}

void APlayerCharacter::ConsumeSprintStamina()
{
    const bool bIsSprinting =
        MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Sprint
        && !GetCharacterMovement()->IsFalling();
    if (bIsSprinting)
    {
        if (GetGroundSpeed() > JogSpeed)
        {
            const float DeltaStamina = SprintStaminaCostPerSecond * SprintStaminaInterval;

            if (!StatsComponent->HasEnoughOrNoRestriction(UStatsComponent::StaminaTag, DeltaStamina))
            {
                // Not enough stamina
                SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Jog);
            }
            else
            {
                // Consume stamina. Refresh Regen timer every time.
                StatsComponent->ModifyCurrentResourceStatChecked(UStatsComponent::StaminaTag, -DeltaStamina, true);
            }
        }
        else // Not fast enough
        {
            // Skip one interval
        }
    }
    else // Not sprinting
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().PauseTimer(SprintStaminaTimerHandle);
        }
    }
}

IInteractable* APlayerCharacter::TraceForInteractables()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    const FVector& Start = FollowCamera->GetComponentLocation();
    const FVector&& End = Start + (FollowCamera->GetForwardVector() * TraceDistance);
    FHitResult HitResult;

    const bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

    return bHit ? Cast<IInteractable>(HitResult.GetActor()) : nullptr;
}

void APlayerCharacter::HandleWeaponChanged(const FWeaponData* WeaponData)
{
    Super::HandleWeaponChanged(WeaponData);

    if (!WeaponData) // Unequipped
    {
        SetOrientRotationToMovement(true);
    }
    else if (TargetingComponent->IsTargetLocked()) // Equipped and target locked
    {
        SetOrientRotationToMovement(false);
    }
}

void APlayerCharacter::HandleInteractable()
{
    IInteractable* NewInteractable = TraceForInteractables();

    if (FocusedInteractable != NewInteractable)
    {
        if (FocusedInteractable)
        {
            FocusedInteractable->ShowHighlight(false);
        }

        // Update widgets bound to delegate.
        FocusedInteractable = NewInteractable;
        OnInteractableFocused.ExecuteIfBound(FocusedInteractable);

        if (FocusedInteractable)
        {
            FocusedInteractable->ShowHighlight(true);
        }
    }
}

void APlayerCharacter::HandleTargetUpdated(AActor* NewActor)
{
    const bool bIsTargetLocked = NewActor != nullptr;
    const bool bEquippedWeapon = EquipmentComponent->GetEquippedItem(DemoGameplayTags::Item_Weapon) != nullptr;
    const bool bShouldFaceTarget = bIsTargetLocked && bEquippedWeapon;
    SetOrientRotationToMovement(!bShouldFaceTarget);

    if (bShouldFaceTarget)
    {
        // @TODO - Enter combat when not ready
    }
}

bool APlayerCharacter::CanReceiveDamageFrom(const AActor* Attacker) const
{
    if (!Super::CanReceiveDamageFrom(Attacker))
    {
        return false;
    }

    // Only from enemies
    if (const APawn* AttackerPawn = Cast<APawn>(Attacker))
    {
        if (const AController* AttackerController = AttackerPawn->GetController())
        {
            return GetTeamAttitudeTowards(*AttackerController) == ETeamAttitude::Hostile;
        }
    }
    return false;
}

FRotator APlayerCharacter::GetDesiredInputRotation() const
{
    if (CachedMoveInputAxis.IsZero())
    {
        return GetActorRotation();
    }

    const FRotator YawRotation = FRotator{0.f, GetControlRotation().Yaw, 0.f};
    const FRotationMatrix RotationMatrix{YawRotation};
    const FVector ForwardDirection = RotationMatrix.GetUnitAxis(EAxis::X);
    const FVector RightDirection = RotationMatrix.GetUnitAxis(EAxis::Y);
    const FVector WorldInput = ForwardDirection * CachedMoveInputAxis.X + RightDirection * CachedMoveInputAxis.Y;

    return UKismetMathLibrary::MakeRotFromX(WorldInput);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Add input mapping context
    if (const APlayerController* PlayerController = GetController<APlayerController>())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Bind input actions
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::MoveComplete);

        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);

        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &ThisClass::StartWalking);
        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &ThisClass::StopWalking);

        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::StartSprinting);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::StopSprinting);

        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);

        EnhancedInputComponent->BindAction(ShowPlayerMenuAction, ETriggerEvent::Started, this, &ThisClass::ShowPlayerMenu);

        EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ThisClass::LightAttack);

        EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &ThisClass::StartBlocking);
        EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ThisClass::StopBlocking);

        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ThisClass::Dodge);

        EnhancedInputComponent->BindAction(ToggleLockOnAction, ETriggerEvent::Started, this, &ThisClass::ToggleLockOn);

        EnhancedInputComponent->BindAction(ToggleHelpTextAction, ETriggerEvent::Started, this, &ThisClass::ToggleHelpText);

        EnhancedInputComponent->BindAction(Test1Action, ETriggerEvent::Started, this, &ThisClass::Test1);
        EnhancedInputComponent->BindAction(Test2Action, ETriggerEvent::Started, this, &ThisClass::Test2);
    }
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MoveAxisVector = Value.Get<FVector2D>();
    CachedMoveInputAxis = MoveAxisVector;

    // Some states don't allow movement input.
    if (!StateManager->CanMoveInCurrentState())
    {
        return;
    }

    const FRotator YawRotation = FRotator{0.f, GetControlRotation().Yaw, 0.f};
    const FRotationMatrix RotationMatrix{YawRotation};
    const FVector ForwardDirection = RotationMatrix.GetUnitAxis(EAxis::X);
    const FVector RightDirection = RotationMatrix.GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MoveAxisVector.X);
    AddMovementInput(RightDirection, MoveAxisVector.Y);
}

void APlayerCharacter::MoveComplete(const FInputActionValue& Value)
{
    CachedMoveInputAxis = FVector2D::ZeroVector;
}

void APlayerCharacter::Jump()
{
    if (!CanPerformJump())
    {
        return;
    }

    StateManager->SetAction(DemoGameplayTags::State_Jump);
    ACharacter::Jump();

    StatsComponent->ConsumeStamina(JumpStaminaCost);

    // Pause SprintStamina
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().PauseTimer(SprintStaminaTimerHandle);
    }
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    // Resume SprintStamina
    UWorld* World = GetWorld();
    if (World && MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Sprint)
    {
        World->GetTimerManager().UnPauseTimer(SprintStaminaTimerHandle);
    }
}

void APlayerCharacter::StartWalking()
{
    if (bIsWalkInputTogglesWalk)
    {
        if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Walk)
        {
            SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Jog);
        }
        else
        {
            SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Walk);
        }
    }
    else
    {
        SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Walk);
    }
}

void APlayerCharacter::StopWalking()
{
    if (bIsWalkInputTogglesWalk)
    {
        // Do nothing
    }
    else
    {
        if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Walk)
        {
            SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Jog);
        }
    }
}

void APlayerCharacter::StartSprinting()
{
    if (bIsSprintInputTogglesSprint)
    {
        if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Sprint)
        {
            SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Jog);
        }
        else
        {
            SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Sprint);
        }
    }
    else
    {
        SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Sprint);
    }
}

void APlayerCharacter::StopSprinting()
{
    if (bIsSprintInputTogglesSprint)
    {
        // Do nothing
    }
    else
    {
        if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Sprint)
        {
            SetMovementSpeedMode(DemoGameplayTags::Movement_SpeedMode_Jog);
        }
    }
}

void APlayerCharacter::Interact()
{
    if (IInteractable* Interactable = FocusedInteractable)
    {
        Interactable->Interact(this);

        // Prevent multiple interactions for TraceInterval.
        FocusedInteractable = nullptr;
        OnInteractableFocused.ExecuteIfBound(nullptr);
    }
}

void APlayerCharacter::ShowPlayerMenu()
{
    if (ADemoPlayerController* DemoPlayerController = GetController<ADemoPlayerController>())
    {
        DemoPlayerController->ShowPlayerMenu(true);
    }
}

void APlayerCharacter::LightAttack()
{
    CombatComponent->Attack(DemoGameplayTags::State_Attack_Light);
}

void APlayerCharacter::Dodge()
{
    PerformAction(DemoGameplayTags::State_Dodge, false, 0);
}

void APlayerCharacter::ToggleLockOn()
{
    TargetingComponent->ToggleTargetLock();
}

void APlayerCharacter::ToggleHelpText()
{
    if (ADemoPlayerController* DemoPlayerController = GetController<ADemoPlayerController>())
    {
        DemoPlayerController->ToggleHelpText();
    }
}

void APlayerCharacter::SetMovementSpeedMode(FGameplayTag NewSpeedMode)
{
    UWorld* World = GetWorld();
    if (!World || MovementSpeedMode == NewSpeedMode)
    {
        return;
    }

    FTimerManager& TimerManager = World->GetTimerManager();

    if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Sprint)
    {
        TimerManager.PauseTimer(SprintStaminaTimerHandle);
    }

    Super::SetMovementSpeedMode(NewSpeedMode);

    if (MovementSpeedMode == DemoGameplayTags::Movement_SpeedMode_Sprint)
    {
        TimerManager.UnPauseTimer(SprintStaminaTimerHandle);
    }
}

void APlayerCharacter::Test1_Implementation()
{
    DemoLOG_CF(LogCharacter, Warning, TEXT("called!"));

    EquipmentComponent->EquipItem(TestItemSlot);
}

void APlayerCharacter::Test2_Implementation()
{
    DemoLOG_CF(LogCharacter, Warning, TEXT("called!"));

    EquipmentComponent->UnequipItem(DemoGameplayTags::Item_Weapon);
    EquipmentComponent->UnequipItem(DemoGameplayTags::Item_Armor_Shield);
}