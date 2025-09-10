// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/ItemTypes.h"
#include "Styling/SlateTypes.h"
#include "ContextMenuWidget.generated.h"

class UButton;
class UImage;
class UVerticalBox;

USTRUCT()
struct FContextAction
{
    GENERATED_BODY()

    FText Label;
};

/**
 * Context menu widget.
 * SetupActions() creates buttons and binds hover events.
 * You should bind click events to the buttons after SetupActions().
 */
UCLASS()
class DEMO_API UContextMenuWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UUserWidget functions
    ////////////////////////////////////////////////////////
protected:
    virtual void NativeOnInitialized() override;

    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    void SetupActions(const TArray<FContextAction>& InActions);

    void ShowContextMenu(const FItemSlot& InSlot, int32 DesignatedIndex);

    void HideContextMenu()
    {
        ClearActionRequest();
        SetVisibility(ESlateVisibility::Hidden);
    }

private:
    void SetPositionToCursor();

    void ClearActionRequest() { ActionRequest.Slot = FItemSlot{}; }

    UFUNCTION()
    void HandleButtonHovered();

    UFUNCTION()
    void HandleButtonUnhovered();

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE TArray<TObjectPtr<UButton>>& GetActionButtons() { return ActionButtons; }

    FORCEINLINE void SetActionRequest(const FItemSlot& InSlot, int32 DesignatedIndex)
    {
        ActionRequest.Slot = InSlot;
        ActionRequest.DesignatedIndex = DesignatedIndex;
        ActionRequest.Quantity = 0;
    }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> FirstDesignButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> MiddleDesignButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> LastDesignButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> ContextVerticalBox;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    TArray<TObjectPtr<UButton>> ActionButtons;

    TArray<TObjectPtr<UImage>> HoveredImages;

    //FButtonStyle FirstButtonStyle;
    //FButtonStyle MiddleButtonStyle;
    //FButtonStyle LastButtonStyle;

private:
    FItemActionRequest ActionRequest;
};