// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CursorWidget.h"
#include "Components/Image.h"
#include "DemoTypes/LogCategories.h"

void UCursorWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    checkf(CursorImage, TEXT("Failed to bind widgets."));

    // Validate images for all cursor states.
    const uint8 BeginIndex = static_cast<uint8>(ECursorState::Default);
    const uint8 EndIndex = static_cast<uint8>(ECursorState::MAX);
    for (uint8 Index = BeginIndex; Index < EndIndex; ++Index)
    {
        const ECursorState CursorState = static_cast<ECursorState>(Index);
        TObjectPtr<UTexture2D>* ImagePtr = CursorImageMap.Find(CursorState);
        if (!ImagePtr || !(*ImagePtr))
        {
            DemoLOG_CF(LogUI, Error, TEXT("Cursor image for state %s is not set."), *UEnum::GetValueAsString(CursorState));
        }
    }
}

void UCursorWidget::UpdateCursorVisuals(ECursorState NewCursorState)
{
    CursorImage->SetBrushFromTexture(CursorImageMap[NewCursorState]);
}