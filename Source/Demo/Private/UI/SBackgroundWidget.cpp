// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SBackgroundWidget.h"
#include "DemoTypes/LogCategories.h"
#include "Settings/DemoProjectSettings.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScaleBox.h"

void SBackgroundWidget::Construct(const FArguments& InArgs)
{
    const UDemoProjectSettings* Settings = GetDefault<UDemoProjectSettings>();
    if (Settings->LoadingScreenImages.IsEmpty())
    {
        DemoLOG_CF(LogUI, Warning, TEXT("No settings or no images found."));
        return;
    }

    // Random image from array.
    int32 ImageIndex = FMath::RandRange(0, Settings->LoadingScreenImages.Num() - 1);

    UObject* ImageObject = Settings->LoadingScreenImages[ImageIndex].TryLoad();
    UTexture2D* Image = Cast<UTexture2D>(ImageObject);

    if (Image)
    {
        ImageBrush = FDeferredCleanupSlateBrush::CreateBrush(Image);

        ChildSlot
            [
                SNew(SBorder)
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Fill)
                    .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                    .BorderBackgroundColor(FLinearColor::Black)
                    [
                        SNew(SScaleBox)
                            .Stretch(Settings->LoadingScreenImageStretch)
                            [
                                SNew(SImage)
                                    .Image(ImageBrush.IsValid() ? ImageBrush->GetSlateBrush() : nullptr)
                            ]
                    ]
            ];
    }
    else
    {
        DemoLOG_CF(LogUI, Warning, TEXT("Failed to load image."));
    }
}