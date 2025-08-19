// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StatsComponent.h"

UStatsComponent::UStatsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UStatsComponent::BeginPlay()
{
    Super::BeginPlay();
}