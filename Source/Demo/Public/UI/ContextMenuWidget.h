// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Widgets/SWidget.h"
#include "ContextMenuWidget.generated.h"

class UImage;
class UTabButton;
class UVerticalBox;

USTRUCT()
struct FContextAction
{
    GENERATED_BODY()

    FText Label;

    FGameplayTag ActionTag;
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

    void ShowContextMenu();

    void HideContextMenu();

private:
    void SetPositionToCursor();

    UFUNCTION()
    void HandleButtonHovered(FGameplayTag InTag);

    UFUNCTION()
    void HandleButtonUnhovered(FGameplayTag InTag);

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    FORCEINLINE TArray<TObjectPtr<UTabButton>>& GetActionButtons() { return ActionButtons; }

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> FirstDesignButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> MiddleDesignButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTabButton> LastDesignButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> ContextVerticalBox;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY()
    TArray<TObjectPtr<UTabButton>> ActionButtons;

    UPROPERTY()
    TArray<TObjectPtr<UImage>> HoveredImages;

private:
    bool bSetupDone{false};

    TWeakPtr<SWidget> PreviousFocus;
};