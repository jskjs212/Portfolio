// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/DemoStateTreeAIComponent.h"
#include "Components/StateTreeComponentSchema.h"
#include "DemoTypes/LogCategories.h"
#include "GameplayTagContainer.h"
#include "StateTree.h"
#include "StateTreeReference.h"

void UDemoStateTreeAIComponent::AddLinkedStateTreeOverridesFixed(const FGameplayTag StateTag, FStateTreeReference StateTreeReference)
{
    // Validate the schema
    if (const UStateTree* ItemStateTree = StateTreeReference.GetStateTree())
    {
        if (ItemStateTree->GetSchema() == nullptr
            || !ItemStateTree->GetSchema()->GetClass()->IsChildOf(UStateTreeComponentSchema::StaticClass()))
        {
            DemoLOG_CF(LogAI, Warning, TEXT("Trying to set the linked overrides with the wrong schema. %s."), *ItemStateTree->GetFullName());
            return;
        }
    }
    LinkedStateTreeOverrides.AddOverride(FStateTreeReferenceOverrideItem(StateTag, MoveTemp(StateTreeReference)));
}