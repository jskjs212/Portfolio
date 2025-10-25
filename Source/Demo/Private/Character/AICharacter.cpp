// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AICharacter.h"
#include "AI/DemoAIController.h"
#include "Character/DemoPawnData.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/LogCategories.h"

#include "Engine/DamageEvents.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AISense_Damage.h"
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

    if (!PawnData)
    {
        return;
    }

    bIsBoss = PawnData->bIsBoss;
    if (ADemoAIController* DemoAIController = GetController<ADemoAIController>())
    {
        DemoAIController->SetIsBoss(bIsBoss);
    }

    // Attach lock-on marker widget to socket
    if (GetMesh()->DoesSocketExist(LockOnSocketName))
    {
        LockOnMarkerWidgetComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LockOnSocketName);
    }
    else
    {
        DemoLOG_CF(LogCharacter, Warning, TEXT("LockOnSocketName '%s' does not exist in '%s'. Using actor location instead."), *LockOnSocketName.ToString(), *GetName());
    }

    if (!bIsBoss)
    {
        // Init AI status widget
        if (UAIStatusWidget* AIStatusWidget = Cast<UAIStatusWidget>(AIStatusWidgetComponent->GetWidget()))
        {
            AIStatusWidget->BindToActor(this);
        }
        else
        {
            DemoLOG_CF(LogCharacter, Warning, TEXT("AIStatusWidget is not valid for %s."), *GetName());
        }
    }
}

float AAICharacter::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    Damage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

    // Notify AI
    FVector HitLocation = PointDamageEvent.HitInfo.ImpactPoint;
    FVector EventLocation = DamageCauser ? DamageCauser->GetActorLocation() : HitLocation;
    UAISense_Damage::ReportDamageEvent(
        this, /* WorldContextObject */
        this, /* DamagedActor */
        EventInstigator,
        Damage,
        EventLocation,
        HitLocation
    );

    return Damage;
}

float AAICharacter::PerformAction(FGameplayTag InAction, bool bIgnoreCurrentState, int32 MontageIndex, bool bUseRandomIndex)
{
    const float Duration = Super::PerformAction(InAction, bIgnoreCurrentState, MontageIndex, bUseRandomIndex);

    if (!StateManager->CanMoveInCurrentState())
    {
        if (ADemoAIController* DemoAIController = GetController<ADemoAIController>())
        {
            DemoAIController->StopMovement();
        }
    }
    return Duration;
}

bool AAICharacter::CanReceiveDamageFrom(const AActor* Attacker) const
{
    if (!Super::CanReceiveDamageFrom(Attacker))
    {
        return false;
    }

    // Only from players
    if (const IGenericTeamAgentInterface* AITeamAgent = GetController<IGenericTeamAgentInterface>())
    {
        return AITeamAgent->GetTeamAttitudeTowards(*Attacker) == ETeamAttitude::Hostile;
    }
    return false;
}

FRotator AAICharacter::GetDesiredControlRotation() const
{
    if (ADemoAIController* DemoAIController = GetController<ADemoAIController>())
    {
        DemoAIController->SetControlRotationToTarget();
    }
    return Super::GetDesiredControlRotation();
}

void AAICharacter::OnTargeted(bool bIsTargeted)
{
    if (!bIsBoss)
    {
        AIStatusWidgetComponent->SetVisibility(bIsTargeted);
    }
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