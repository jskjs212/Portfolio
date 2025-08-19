// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

APlayerCharacter::APlayerCharacter()
{
    // Don't follow camera rotation
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

    // TODO:
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();

    // TODO:
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MoveVector = Value.Get<FVector2D>();

    // TODO:
}