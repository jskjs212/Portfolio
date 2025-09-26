// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AICharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoGameplayTags.h"

// @TEST
#include "Components/StatsComponent.h"
#include "UI/AIStatusWidget.h"

AAICharacter::AAICharacter()
{
    AIStatusWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AIStatusWidget"));
    AIStatusWidgetComponent->SetupAttachment(GetMesh());
    AIStatusWidgetComponent->SetRelativeLocation(FVector{0.f, 0.f, 220.f});
    AIStatusWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // @TODO - Face the player when targeted?
    AIStatusWidgetComponent->SetDrawSize(FVector2D{180.f, 45.f});
    AIStatusWidgetComponent->SetVisibility(false);
}

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!GetMesh()->DoesSocketExist(LockOnSocketName))
    {
        UE_LOG(LogTemp, Warning, TEXT("LockOnSocketName '%s' does not exist in '%s'. Using actor location instead."),
            *LockOnSocketName.ToString(), *GetName());
    }

    // @debug
    //if (!BehaviorTreeOverride)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("This AICharacter (%s) is using default BehaviorTree."), *GetName());
    //}

    if (UAIStatusWidget* AIStatusWidget = Cast<UAIStatusWidget>(AIStatusWidgetComponent->GetWidget()))
    {
        AIStatusWidget->InitAIStatus(this);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AIStatusWidget is not valid for %s."), *GetName());
    }
}

void AAICharacter::OnTargeted(bool bIsTargeted)
{
    // @TODO
    AIStatusWidgetComponent->SetVisibility(bIsTargeted);
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