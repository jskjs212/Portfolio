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
    //        Statics
    ////////////////////////////////////////////////////////
public:
    // check: What if AItem is inherited by AWeapon, AArmor, etc.?
    static AItem* SpawnItem(
        UWorld* World,
        const FItemSlot& InSlot,
        const FTransform& SpawnTransform,
        bool bDisableCollision = false,
        AActor* Owner = nullptr,
        APawn* Instigator = nullptr,
        ESpawnActorCollisionHandlingMethod CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

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
    bool IsStaticMeshValid() const;

    //UStaticMeshComponent* GetStaticMesh() const { return StaticMesh; }

    FORCEINLINE FItemSlot& GetItemSlot() { return ItemSlot; }

    FORCEINLINE void SetItemSlot(const FItemSlot& InItemSlot) { ItemSlot = InItemSlot; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
private:
    UPROPERTY(EditAnywhere, Category = "Item")
    FItemSlot ItemSlot;
};