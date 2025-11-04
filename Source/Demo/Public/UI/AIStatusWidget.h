// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AIStatusWidget.generated.h"

class AController;
class UDamageType;
class UStatBarWidget;
class UTextBlock;

/**
 *
 */
UCLASS()
class DEMO_API UAIStatusWidget : public UUserWidget
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        UI functions
    ////////////////////////////////////////////////////////
public:
    UAIStatusWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;

public:
    // Unbind from previous actor if already bound.
    void BindToActor(AActor* InActor);

    void UnbindFromActor();

private:
    void HideDamageText();

    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

    ////////////////////////////////////////////////////////
    //        Widgets
    ////////////////////////////////////////////////////////
protected:
    // StatBarWidget handles unbinding itself.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UStatBarWidget> HealthBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NameText;

    // Simple damage indicator UI
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DamageText;

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Initialization", meta = (ClampMin = "0.1"))
    float DamageTextDuration{1.5f};

    FTimerHandle DamageTextHideTimerHandle;
    FTimerDelegate DamageTextHideTimerDelegate;

    TWeakObjectPtr<AActor> CachedActor;
};