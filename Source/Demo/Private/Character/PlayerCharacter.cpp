// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

APlayerCharacter::APlayerCharacter()
{
    // Disable character rotation by controller
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator{0.f, 540.f, 0.f};

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Add input mapping context
    if (APlayerController* PlayerController = GetController<APlayerController>())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
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
    if (bIsWalkInputTogglesWalking)
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
    if (bIsWalkInputTogglesWalking)
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
    if (bIsSprintInputTogglesSprinting)
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
    if (bIsSprintInputTogglesSprinting)
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

void APlayerCharacter::Test_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::Test() called!"));
}