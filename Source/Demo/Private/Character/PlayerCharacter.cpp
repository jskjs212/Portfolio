// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Items/Item.h"
#include "Items/ItemTypes.h"
#include "PlayerController/DemoPlayerController.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Disable character rotation by controller
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    check(CapsuleComp);
    CapsuleComp->InitCapsuleSize(42.f, 96.f);

    // Adjust for Elf_Arden mesh
    USkeletalMeshComponent* MeshComp = GetMesh();
    check(MeshComp);
    MeshComp->SetRelativeLocation(FVector{0.f, 0.f, -96.f});
    MeshComp->SetRelativeRotation(FRotator{0.f, -90.f, 0.f});

    UCharacterMovementComponent* MovementComp = GetCharacterMovement();
    MovementComp->bUseControllerDesiredRotation = false;
    MovementComp->bOrientRotationToMovement = true;
    MovementComp->RotationRate = FRotator{0.f, 540.f, 0.f};

    // TODO: Use data table or config file
    StatsComponent->RemoveResourceStat(UStatsComponent::HealthTag);
    StatsComponent->AddResourceStat(UStatsComponent::HealthTag, FResourceStat{100.f, 100.f, true});
    StatsComponent->AddResourceStat(UStatsComponent::StaminaTag, FResourceStat{100.f, 100.f, true, 10.f});

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Camera position
    CameraBoom->SetRelativeLocation(FVector{0.f, 0.f, 160.f});
    CameraBoom->SocketOffset = FVector{0.f, 50.f, 0.f};

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Init HUD
    if (ADemoPlayerController* DemoPlayerController = GetController<ADemoPlayerController>())
    {
        DemoPlayerController->InitDemoHUD();
    }

    // Trace for interactables at intervals rather than every tick.
    if (UWorld* World = GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TraceTimerHandle, this, &ThisClass::HandleInteractable, TraceInterval, true);
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

        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);

        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &ThisClass::StartWalk);
        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &ThisClass::StopWalk);

        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::StartSprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);

        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);

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
    const FRotator YawRotation = FRotator{0.f, GetControlRotation().Yaw, 0.f};
    const FRotationMatrix RotationMatrix{YawRotation};
    const FVector ForwardDirection = RotationMatrix.GetUnitAxis(EAxis::X);
    const FVector RightDirection = RotationMatrix.GetUnitAxis(EAxis::Y);

    const FVector2D MoveAxisVector = Value.Get<FVector2D>();
    AddMovementInput(ForwardDirection, MoveAxisVector.Y);
    AddMovementInput(RightDirection, MoveAxisVector.X);
}

void APlayerCharacter::StartWalk()
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

void APlayerCharacter::StopWalk()
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

void APlayerCharacter::StartSprint()
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

    // TODO: Consume stamina if sprinting
}

void APlayerCharacter::StopSprint()
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

void APlayerCharacter::Test1_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Test1() called!"));

    UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::Test1() - EquipItem"));
    TestWeapon.Quantity = 1;
    TestShield.Quantity = 1;
    EquipmentComponent->EquipItem(TestWeapon);
    EquipmentComponent->EquipItem(TestShield);

    UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::Test1() - AddItem"));
    TestFood.Quantity = 3;
    InventoryComponent->AddItem(TestFood, 5);
    InventoryComponent->AddItem(TestFood, 6);

    UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::Test1() - UseItem - Food"));
    FItemActionRequest Request;
    StatsComponent->TakeDamage(30.f);
    Request.Slot = TestFood;
    Request.DesignatedIndex = 5;
    Request.Quantity = 2;
    InventoryComponent->UseItem(Request);

    InventoryComponent->SwapItem(DemoGameplayTags::Item_Consumable, 5, 6);
    InventoryComponent->AddMaxSlotSize(DemoGameplayTags::Item_Consumable, 1);
}

void APlayerCharacter::Test2_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Test2() called!"));

    UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::Test2() - UnequipItem"));
    TestWeapon.Quantity = 1;
    TestShield.Quantity = 1;
    EquipmentComponent->UnequipItem(DemoGameplayTags::Item_Weapon);
    EquipmentComponent->UnequipItem(DemoGameplayTags::Item_Armor_Shield);

    UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::Test2() - AddItem"));
    InventoryComponent->AddItem(TestWeapon, 6);

    UE_LOG(LogTemp, Display, TEXT("APlayerCharacter::Test2() - DropItem"));
    FItemActionRequest Request;
    Request.Slot = TestWeapon;
    Request.DesignatedIndex = 6;
    Request.Quantity = 1;
    InventoryComponent->DropItem(Request);
}