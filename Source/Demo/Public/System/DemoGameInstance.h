// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DemoGameInstance.generated.h"

/**
 *
 */
UCLASS()
class DEMO_API UDemoGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION()
    void BeginLoadingScreen(const FString& InMapName);

    UFUNCTION()
    void EndLoadingScreen(UWorld* InLoadedWorld);
};