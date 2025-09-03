// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UIManagementSubsystem.h"
#include "Items/Item.h"
#include "UI/InteractWidget.h"

UUIManagementSubsystem::UUIManagementSubsystem()
{
    // check: Hardcoded path -> config file?
    static ConstructorHelpers::FClassFinder<UInteractWidget> InteractWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_Interact.WBP_Interact_C'"));
    if (InteractWidgetBPClass.Succeeded())
    {
        InteractWidgetClass = InteractWidgetBPClass.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UUIManagementSubsystem - InteractWidget BP not found."));
    }
}

void UUIManagementSubsystem::Init()
{
    InitInteractWidget();
}

void UUIManagementSubsystem::InitInteractWidget()
{
    if (InteractWidgetClass)
    {
        InteractWidget = CreateWidget<UInteractWidget>(GetWorld(), InteractWidgetClass);
        if (InteractWidget)
        {
            InteractWidget->SetVisibility(ESlateVisibility::Hidden);
            InteractWidget->AddToViewport();
        }
    }
}

void UUIManagementSubsystem::ShowInteractWidget(const AItem* Item)
{
    // On
    if (IsValid(Item))
    {
        if (Item != CachedShowInfoItem)
        {
            CachedShowInfoItem = Item;
            InteractWidget->UpdateUI(Item->GetItemSlot());
            InteractWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }
    // Off
    else if (CachedShowInfoItem)
    {
        CachedShowInfoItem = nullptr;
        InteractWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}
