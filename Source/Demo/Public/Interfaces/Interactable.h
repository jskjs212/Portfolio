// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class DEMO_API IInteractable
{
    GENERATED_BODY()

public:
    virtual void Interact(APawn* InstigatorPawn) PURE_VIRTUAL(IInteractable::Interact);

    virtual void ShowHighlight(bool bShow) PURE_VIRTUAL(IInteractable::ShowHighlight);
};