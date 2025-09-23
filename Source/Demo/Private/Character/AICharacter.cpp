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
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(GetMesh());
    WidgetComponent->SetRelativeLocation(FVector{0.f, 0.f, 220.f});
    //WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // @TODO - Face the player when targeted?
    WidgetComponent->SetDrawSize(FVector2D{180.f, 45.f});
    WidgetComponent->SetVisibility(false);
}

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();

    // @debug
    //if (!BehaviorTreeOverride)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("This AICharacter (%s) is using default BehaviorTree."), *GetName());
    //}

    if (UAIStatusWidget* AIStatusWidget = Cast<UAIStatusWidget>(WidgetComponent->GetWidget()))
    {
        AIStatusWidget->InitAIStatus(this);
    }
}

bool AAICharacter::CanBeTargeted() const
{
    if (StateManager->IsInState(DemoGameplayTags::State_Dead))
    {
        return false;
    }

    return true;
}

void AAICharacter::OnTargeted(bool bIsTargeted)
{
    UE_LOG(LogTemp, Warning, TEXT("AICharacter (%s) has been %s."), *GetName(), bIsTargeted ? TEXT("targeted") : TEXT("untargeted"));

    // @TODO
    WidgetComponent->SetVisibility(bIsTargeted);
}