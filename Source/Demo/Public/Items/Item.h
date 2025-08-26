// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemTypes.h"
#include "Item.generated.h"

class UStaticMeshComponent;

UCLASS()
class DEMO_API AItem : public AActor
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    // Only static mesh for now.
    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    AItem();

protected:
    virtual void BeginPlay() override;

public:
    virtual void OnConstruction(const FTransform& Transform) override;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    UStaticMeshComponent* GetStaticMesh() const { return StaticMesh; }

    FItemSlot& GetItemSlot() { return ItemSlot; }

    void SetItemSlot(const FItemSlot& InItemSlot) { ItemSlot = InItemSlot; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Item")
    FItemSlot ItemSlot;
};