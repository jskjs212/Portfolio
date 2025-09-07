// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/DemoHUD.h"
#include "UI/DemoHUDWidget.h"
#include "UI/PlayerMenuWidget.h"

ADemoHUD::ADemoHUD()
{
    // hardcoded:
    static ConstructorHelpers::FClassFinder<UDemoHUDWidget> DemoHUDWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_DemoHUD.WBP_DemoHUD_C'"));
    if (DemoHUDWidgetBPClass.Succeeded())
    {
        DemoHUDWidgetClass = DemoHUDWidgetBPClass.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ADemoHUD - DemoHUDWidget BP not found."));
        DemoHUDWidgetClass = UDemoHUDWidget::StaticClass();
    }
}

void ADemoHUD::Init()
{
    APlayerController* PlayerController = GetOwningPlayerController();
    if (!PlayerController || !DemoHUDWidgetClass)
    {
        return;
    }

    DemoHUDWidget = CreateWidget<UDemoHUDWidget>(PlayerController, DemoHUDWidgetClass);
    if (DemoHUDWidget)
    {
        DemoHUDWidget->AddToViewport(0);
    }
}

//void ADemoHUD::SetHUDVisibility(bool bVisible)
//{
//    if (DemoHUDWidget)
//    {
//        DemoHUDWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
//    }
//}