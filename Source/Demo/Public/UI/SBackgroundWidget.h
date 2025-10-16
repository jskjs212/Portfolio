// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FDeferredCleanupSlateBrush;
class UTexture2D;

/**
 *
 */
class DEMO_API SBackgroundWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SBackgroundWidget) {}

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TSharedPtr<FDeferredCleanupSlateBrush> ImageBrush;
};