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
#include "StateTreeEvents.h"
#include "StateTreeReference.h"

ADemoAIController::ADemoAIController()
{
    SetGenericTeamId(Demo::TeamID::Enemy); // @TODO - Enemy and Neutral

    PeacefulStateTreeTag = DemoGameplayTags::StateTree_Tree_Peaceful;
    AggressiveStateTreeTag = DemoGameplayTags::StateTree_Tree_Aggressive;

    DemoStateTreeAIComponent = CreateDefaultSubobject<UDemoStateTreeAIComponent>(TEXT("DemoStateTreeAIComponent"));
    DemoStateTreeAIComponent->SetStartLogicAutomatically(false);

    DemoPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    SetPerceptionComponent(*DemoPerceptionComponent);

    UAISenseConfig_Damage* SenseDamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("SenseDamageConfig"));
    SenseDamageConfig->SetMaxAge(0.f); // Only sight sense will unset the target.

    UAISenseConfig_Sight* SenseSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SenseSightConfig"));
    SenseSightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SenseSightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    SenseSightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    SenseSightConfig->SightRadius = 900.f;
    SenseSightConfig->LoseSightRadius = 1300.f;
    SenseSightConfig->PeripheralVisionAngleDegrees = 75.f;
    SenseSightConfig->PointOfViewBackwardOffset = 100.f;
    SenseSightConfig->NearClippingRadius = 0.f;
    SenseSightConfig->AutoSuccessRangeFromLastSeenLocation = 450.f;
    SenseSightConfig->SetMaxAge(8.f);

    DemoPerceptionComponent->ConfigureSense(*SenseDamageConfig);
    DemoPerceptionComponent->ConfigureSense(*SenseSightConfig);
    DemoPerceptionComponent->SetDominantSense(SenseSightConfig->GetSenseImplementation());

    DemoPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionUpdated);
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
            // If the current state disallows movement, deny the request for AAIController::MoveTo.
            if (!StateManager->CanMoveInCurrentState())
            {
                FPathFollowingRequestResult Result;
                Result.Code = EPathFollowingRequestResult::Failed;
                if (GetPathFollowingComponent())
                {
                    Result.MoveId = GetPathFollowingComponent()->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
                }

                // If this happens infinitely, state in StateTree or StateManagerComponent is not handled properly.
                // Or AI is just trying to move while stunned.
                DemoLOG_CF(LogAI, Display, TEXT("Request denied. Pawn: %s, CurrentState: %s"), *MyPawn->GetName(), *StateManager->GetCurrentState().ToString());
                return Result;
            }
        }
    }
    return Super::MoveTo(MoveRequest, OutPath);
}

void ADemoAIController::SendStateTreeEvent(FGameplayTag InTag, FName InOrigin)
{
    if (DemoStateTreeAIComponent->IsRunning())
    {
        FStateTreeEvent Event;
        Event.Tag = InTag;
        Event.Origin = InOrigin;

        DemoStateTreeAIComponent->SendStateTreeEvent(Event);
    }
}

void ADemoAIController::OverrideStateTree(FStateTreeReference InStateTreeRef, FGameplayTag InStateTreeTag)
{
    if (InStateTreeRef.IsValid())
    {
        if (DemoStateTreeAIComponent->IsRunning())
        {
            DemoStateTreeAIComponent->StopLogic(TEXT("ADemoAIController::OverrideStateTree"));
        }

        DemoStateTreeAIComponent->RemoveLinkedStateTreeOverrides(InStateTreeTag);
        DemoStateTreeAIComponent->AddLinkedStateTreeOverrides_Fixed(InStateTreeTag, InStateTreeRef);
        DemoStateTreeAIComponent->StartLogic();
    }
    else
    {
        DemoLOG_CF(LogAI, Display, TEXT("StateTreeReference is not valid. Pawn: %s, Tag: %s"), *GetNameSafe(GetPawn()), *InStateTreeTag.ToString());
    }
}

void ADemoAIController::OverrideStateTree(const APawn* InPawn)
{
    if (const AAICharacter* AICharacter = Cast<AAICharacter>(InPawn))
    {
        const FStateTreeReference& Peaceful = AICharacter->GetStateTreeRefOverride_Peaceful();
        if (Peaceful.IsValid())
        {
            OverrideStateTree(AICharacter->GetStateTreeRefOverride_Peaceful(), PeacefulStateTreeTag);
        }

        const FStateTreeReference& Aggressive = AICharacter->GetStateTreeRefOverride_Aggressive();
        if (Aggressive.IsValid())
        {
            OverrideStateTree(AICharacter->GetStateTreeRefOverride_Aggressive(), AggressiveStateTreeTag);
        }
    }
    else
    {
        DemoLOG_CF(LogAI, Warning, TEXT("Possessed pawn (%s) is not an AAICharacter."), *GetNameSafe(InPawn));
    }
}

void ADemoAIController::HandleTargetPerceptionUpdated(AActor* InActor, FAIStimulus Stimulus)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
    // @debug - Uncomment if needed.
    //DemoLOG_F(LogAI, Display, TEXT("Pawn: %s, Sensed: %s, Stimulus: %s"),
    //    *GetNameSafe(GetPawn()),
    //    Stimulus.WasSuccessfullySensed() ? TEXT("true") : TEXT("false"),
    //    *Stimulus.GetDebugDescription()
    //);
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

    if (bIsBoss)
    {
        // Boss AI ignores perception updates.
        // @TODO - Implement BossAIController
        return;
    }

    // Set TargetActor, and send StateTree event.
    if (Stimulus.WasSuccessfullySensed()) // Target sensed
    {
        TargetActor = InActor;
        SendStateTreeEvent(DemoGameplayTags::StateTree_Event_TargetSensed, TEXT("ADemoAIController::HandleTargetPerceptionInfoUpdated"));
    }
    else if (TargetActor == InActor) // Target lost
    {
        TargetActor = nullptr;
        SendStateTreeEvent(DemoGameplayTags::StateTree_Event_TargetLost, TEXT("ADemoAIController::HandleTargetPerceptionInfoUpdated"));
    }
}

void ADemoAIController::SetTargetActorManually(AActor* InTargetActor)
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
    const FGameplayTag EventTag = InTargetActor ? DemoGameplayTags::StateTree_Event_TargetSensed : DemoGameplayTags::StateTree_Event_TargetLost;
    SendStateTreeEvent(EventTag, TEXT("ADemoAIController::SetTargetActorManually"));
}

void ADemoAIController::SetControlRotationToTarget()
{
    const APawn* MyPawn = GetPawn();
    if (MyPawn && TargetActor.IsValid())
    {
        const FVector Direction = TargetActor->GetActorLocation() - MyPawn->GetActorLocation();
        const FRotator DesiredRotation = UKismetMathLibrary::MakeRotFromX(Direction);
        SetControlRotation(DesiredRotation);
    }
}