// Fill out your copyright notice in the Description page of Project Settings.

#include "Input/DemoInputHelper.h"
#include "DemoTypes/LogCategories.h"
#include "Input/DemoInputMappingCollection.h"
#include "InputMappingContext.h"
#include "Settings/DemoProjectSettings.h"

namespace Demo::Input
{
TPair<const UInputMappingContext*, int32> FindInputMappingContext(FGameplayTag ContextTag)
{
    const UDemoInputMappingCollection* InputMappingCollection = GetDefault<UDemoProjectSettings>()->InputMappingCollection.LoadSynchronous();
    if (!InputMappingCollection)
    {
        DemoLOG_CF(LogInput, Error, TEXT("InputMappingCollection is not set in DemoProjectSettings."));
        return {nullptr, 0};
    }

    const FInputMappingContextData* InputMappingContextData = InputMappingCollection->GetInputMappingData(ContextTag);
    if (!InputMappingContextData)
    {
        DemoLOG_CF(LogInput, Error, TEXT("InputMappingContextData is not found. Tag: %s"), *ContextTag.ToString());
        return {nullptr, 0};
    }

    const UInputMappingContext* InputMappingContext = InputMappingContextData->InputMappingContext.LoadSynchronous();
    if (!InputMappingContext)
    {
        DemoLOG_CF(LogInput, Error, TEXT("Failed to load InputMappingContext. Tag: %s"), *ContextTag.ToString());
        return {nullptr, 0};
    }

    return {InputMappingContext, InputMappingContextData->Priority};
}
} // namespace Demo::Input