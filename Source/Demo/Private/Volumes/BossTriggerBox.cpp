// Fill out your copyright notice in the Description page of Project Settings.

#include "Volumes/BossTriggerBox.h"
#include "Character/PlayerCharacter.h"
#include "Components/BossEncounterComponent.h"
#include "DemoTypes/LogCategories.h"

ABossTriggerBox::ABossTriggerBox()
{
    PrimaryActorTick.bCanEverTick = false;

    BossEncounterComponent = CreateDefaultSubobject<UBossEncounterComponent>(TEXT("BossEncounterComponent"));
}

void ABossTriggerBox::BeginPlay()
{
    Super::BeginPlay();

    if (!BossPawn.IsValid())
    {
        DemoLOG_CF(LogDemoGame, Error, TEXT("No BossPawn assigned for %s."), *GetName());
        return; // @check - Set boss pawn at runtime?
    }
    if (!BossMusicTag.IsValid())
    {
        DemoLOG_CF(LogDemoGame, Warning, TEXT("No BossMusicTag assigned for %s."), *GetName());
        // Continue
    }

    BossEncounterComponent->SetupBossInfo(BossPawn.Get(), BossMusicTag);

    OnActorBeginOverlap.AddDynamic(this, &ThisClass::HandleBeginOverlap);
    OnActorEndOverlap.AddDynamic(this, &ThisClass::HandleEndOverlap);
}

void ABossTriggerBox::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    // Start the encounter only if the actor is APlayerCharacter.
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
    {
        BossEncounterComponent->StartEncounter(PlayerCharacter);
    }
}

void ABossTriggerBox::HandleEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor))
    {
        BossEncounterComponent->EndEncounter(EBossEncounterEndReason::PlayerRetreated);
    }
}