// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DemoAIController.h"
#include "Character/AICharacter.h"
#include "Components/DemoStateTreeAIComponent.h"
#include "DemoTypes/DemoTypes.h"
#include "DemoTypes/LogCategories.h"
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
    if (UpdateInfo.Stimulus.WasSuccessfullySensed())
    {
        TargetActor = UpdateInfo.Target.Get();

        if (DemoStateTreeAIComponent->IsRunning())
        {
            FStateTreeEvent Event{DemoGameplayTags::StateTree_Event_TargetSensed};
            Event.Origin = TEXT("DemoAIController::HandleTargetPerceptionInfoUpdated");
            DemoStateTreeAIComponent->SendStateTreeEvent(Event);
        }
    }
    else if (TargetActor == UpdateInfo.Target.Get())
    {
        TargetActor = nullptr;

        if (DemoStateTreeAIComponent->IsRunning())
        {
            FStateTreeEvent Event{DemoGameplayTags::StateTree_Event_TargetLost};
            Event.Origin = TEXT("DemoAIController::HandleTargetPerceptionInfoUpdated");
            DemoStateTreeAIComponent->SendStateTreeEvent(Event);
        }
    }
}