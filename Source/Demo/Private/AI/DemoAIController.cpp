// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/DemoAIController.h"
#include "Character/AICharacter.h"
#include "Components/DemoStateTreeAIComponent.h"
#include "Components/StateManagerComponent.h"
#include "DemoTypes/DemoTypes.h"
#include "DemoTypes/LogCategories.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "StateTreeReference.h"

ADemoAIController::ADemoAIController()
{
    SetGenericTeamId(DemoTeamID::Enemy); // @TODO - Enemy and Neutral

    DemoStateTreeAIComponent = CreateDefaultSubobject<UDemoStateTreeAIComponent>(TEXT("DemoStateTreeAIComponent"));
    DemoStateTreeAIComponent->SetStartLogicAutomatically(false);

    DemoPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*DemoPerceptionComponent);

    SenseDamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("SenseDamageConfig"));

    SenseSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SenseSightConfig"));
    SenseSightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SenseSightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    SenseSightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SenseSightConfig->SightRadius = 1000.f;
    SenseSightConfig->LoseSightRadius = 1500.f;
    SenseSightConfig->PeripheralVisionAngleDegrees = 45.f;
    SenseSightConfig->AutoSuccessRangeFromLastSeenLocation = 300.f;

    DemoPerceptionComponent->ConfigureSense(*SenseDamageConfig);
    DemoPerceptionComponent->ConfigureSense(*SenseSightConfig);
    DemoPerceptionComponent->SetDominantSense(SenseSightConfig->GetSenseImplementation());

    DemoPerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionInfoUpdated);
}

void ADemoAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    OverrideStateTree(InPawn);
}

FPathFollowingRequestResult ADemoAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
    if (APawn* MyPawn = GetPawn())
    {
        if (const UStateManagerComponent* StateManager = MyPawn->FindComponentByClass<UStateManagerComponent>())
        {
            if (!StateManager->CanMoveInCurrentState())
            {
                FPathFollowingRequestResult Result;
                Result.Code = EPathFollowingRequestResult::Failed;
                if (GetPathFollowingComponent())
                {
                    Result.MoveId = GetPathFollowingComponent()->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
                }
                return Result;
            }
        }
    }
    return Super::MoveTo(MoveRequest, OutPath);
}

void ADemoAIController::SetControlRotationToTarget()
{
    const APawn* MyPawn = GetPawn();
    if (MyPawn && TargetActor)
    {
        const FVector Direction = TargetActor->GetActorLocation() - MyPawn->GetActorLocation();
        const FRotator DesiredRotation = UKismetMathLibrary::MakeRotFromX(Direction);
        SetControlRotation(DesiredRotation);
    }
}

void ADemoAIController::OverrideStateTree(FStateTreeReference InStateTreeRef)
{
    if (DemoStateTreeAIComponent->IsRunning())
    {
        DemoStateTreeAIComponent->StopLogic(TEXT("Changing StateTreeRefOverride."));
    }

    if (InStateTreeRef.IsValid())
    {
        DemoStateTreeAIComponent->RemoveLinkedStateTreeOverrides(StateTreeTag);
        DemoStateTreeAIComponent->AddLinkedStateTreeOverrides_Fixed(StateTreeTag, InStateTreeRef);
        DemoStateTreeAIComponent->StartLogic();
    }
    else
    {
        DemoLOG_CF(LogAI, Verbose, TEXT("%s has no valid StateTreeRefOverride. Stop logic."), *GetNameSafe(GetPawn()));
    }
}

void ADemoAIController::OverrideStateTree(const APawn* InPawn)
{
    if (const AAICharacter* AICharacter = Cast<AAICharacter>(InPawn))
    {
        OverrideStateTree(AICharacter->GetStateTreeRefOverride());
    }
    else
    {
        DemoLOG_CF(LogAI, Warning, TEXT("Possessed pawn (%s) is not an AAICharacter."), *GetNameSafe(InPawn));
    }
}

void ADemoAIController::HandleTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
    if (bIsBoss)
    {
        // Boss AI ignores perception updates.
        // @TODO - Make BossAIController, override StateTree, or use different perception settings.
        return;
    }

    // Set TargetActor, and send StateTree event.
    if (UpdateInfo.Stimulus.WasSuccessfullySensed()) // Target sensed
    {
        TargetActor = UpdateInfo.Target.Get();

        if (DemoStateTreeAIComponent->IsRunning())
        {
            FStateTreeEvent Event;
            Event.Tag = DemoGameplayTags::StateTree_Event_TargetSensed;
            Event.Origin = TEXT("ADemoAIController::HandleTargetPerceptionInfoUpdated");

            DemoStateTreeAIComponent->SendStateTreeEvent(Event);
        }
    }
    else if (TargetActor == UpdateInfo.Target.Get()) // Target lost
    {
        TargetActor = nullptr;

        if (DemoStateTreeAIComponent->IsRunning())
        {
            FStateTreeEvent Event;
            Event.Tag = DemoGameplayTags::StateTree_Event_TargetLost;
            Event.Origin = TEXT("ADemoAIController::HandleTargetPerceptionInfoUpdated");

            DemoStateTreeAIComponent->SendStateTreeEvent(Event);
        }
    }
}

void ADemoAIController::SetTargetActor(AActor* InTargetActor)
{
    if (!bIsBoss)
    {
        // AI perception will interfere.
        DemoLOG_CF(LogAI, Warning, TEXT("Called on non-boss AI (%s)."), *GetNameSafe(GetPawn()));
    }

    if (TargetActor == InTargetActor)
    {
        return;
    }
    TargetActor = InTargetActor;

    // Send StateTree event
    if (DemoStateTreeAIComponent->IsRunning())
    {
        FStateTreeEvent Event;
        if (TargetActor)
        {
            Event.Tag = DemoGameplayTags::StateTree_Event_TargetSensed;
            Event.Origin = TEXT("ADemoAIController::SetTargetActor");
        }
        else
        {
            Event.Tag = DemoGameplayTags::StateTree_Event_TargetLost;
            Event.Origin = TEXT("ADemoAIController::SetTargetActor");
        }
        DemoStateTreeAIComponent->SendStateTreeEvent(Event);
    }
}