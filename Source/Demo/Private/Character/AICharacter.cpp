// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AICharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"
#include "UI/AIStatusWidget.h"

AAICharacter::AAICharacter()
{
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    AIStatusWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AIStatusWidget"));
    AIStatusWidgetComponent->SetupAttachment(GetMesh());
    AIStatusWidgetComponent->SetRelativeLocation(FVector{0.f, 0.f, 220.f});
    AIStatusWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    AIStatusWidgetComponent->SetDrawSize(FVector2D{190.f, 50.f});
    AIStatusWidgetComponent->SetVisibility(false);

    LockOnMarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnMarkerWidget"));
    LockOnMarkerWidgetComponent->SetupAttachment(GetMesh());
    LockOnMarkerWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    LockOnMarkerWidgetComponent->SetDrawSize(FVector2D{20.f, 20.f});
    LockOnMarkerWidgetComponent->SetVisibility(false);
}

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();

    // Attach lock-on marker widget to socket
    if (GetMesh()->DoesSocketExist(LockOnSocketName))
    {
        // @TEST
        DemoLOG_CF(LogCharacter, Log, TEXT("Attaching LockOnMarkerWidget to socket '%s' of '%s'."), *LockOnSocketName.ToString(), *GetName());
        LockOnMarkerWidgetComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LockOnSocketName);
    }
    else
    {
        DemoLOG_CF(LogCharacter, Warning, TEXT("LockOnSocketName '%s' does not exist in '%s'. Using actor location instead."), *LockOnSocketName.ToString(), *GetName());
    }

    // Init AI status widget
    if (UAIStatusWidget* AIStatusWidget = Cast<UAIStatusWidget>(AIStatusWidgetComponent->GetWidget()))
    {
        AIStatusWidget->InitAIStatus(this);
    }
    else
    {
        DemoLOG_CF(LogCharacter, Warning, TEXT("AIStatusWidget is not valid for %s."), *GetName());
    }
}

void AAICharacter::OnTargeted(bool bIsTargeted)
{
    AIStatusWidgetComponent->SetVisibility(bIsTargeted);
    LockOnMarkerWidgetComponent->SetVisibility(bIsTargeted);
}

bool AAICharacter::CanBeTargeted() const
{
    if (StateManager->IsInState(DemoGameplayTags::State_Dead))
    {
        return false;
    }
    return true;
}

FVector AAICharacter::GetTargetPointLocation() const
{
    if (USkeletalMeshComponent* MeshComponent = GetMesh())
    {
        return MeshComponent->GetSocketLocation(LockOnSocketName);
    }
    return GetActorLocation();
}