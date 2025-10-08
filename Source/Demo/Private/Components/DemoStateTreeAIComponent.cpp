// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/DemoStateTreeAIComponent.h"
#include "Components/StateTreeComponentSchema.h"
#include "DemoTypes/LogCategories.h"
#include "GameplayTagContainer.h"
#include "StateTree.h"
#include "StateTreeSchema.h"
#include "StateTreeReference.h"

void UDemoStateTreeAIComponent::AddLinkedStateTreeOverrides_Fixed(const FGameplayTag StateTag, FStateTreeReference StateTreeReference)
{
    // Validate the schema
    if (const UStateTree* ItemStateTree = StateTreeReference.GetStateTree())
    {
        TSubclassOf<UStateTreeComponentSchema> BaseSchema = UStateTreeComponentSchema::StaticClass();
        if (const UStateTree* BaseStateTreePtr = StateTreeRef.GetStateTree())
        {
            if (const UStateTreeSchema* BaseSchemaPtr = BaseStateTreePtr->GetSchema())
            {
                BaseSchema = BaseSchemaPtr->GetClass();
            }
        }

        if (ItemStateTree->GetSchema() == nullptr
            || !ItemStateTree->GetSchema()->GetClass()->IsChildOf(BaseSchema.Get()))
        {
            DemoLOG_CF(LogAI, Warning, TEXT("Trying to set the linked overrides with the wrong schema: %s (expected: %s)."),
                *ItemStateTree->GetFullName(),
                *BaseSchema->GetName()
            );
            return;
        }
    }
    LinkedStateTreeOverrides.AddOverride(FStateTreeReferenceOverrideItem(StateTag, MoveTemp(StateTreeReference)));
}