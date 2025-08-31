// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemTypes.h"
#include "Item.generated.h"

class UMeshComponent;
class USceneComponent;
class USkeletalMeshComponent;
class USphereComponent;
class UStaticMeshComponent;

UENUM()
enum class EItemMeshType : uint8
{
    None,
    StaticMesh,
    SkeletalMesh
};

UCLASS()
class DEMO_API AItem : public AActor
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    //TObjectPtr<USceneComponent> DefaultSceneRoot;

    // Collision for item pickup.
    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<USphereComponent> AreaSphere;

    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

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
    // Create mesh component based on the mesh asset type.
    // Assume that this is called in the editor, or once when spawned in game.
    virtual void OnConstruction(const FTransform& Transform) override;

    ////////////////////////////////////////////////////////
    //        Item functions
    ////////////////////////////////////////////////////////
protected:
    void SetupMesh();

    void DisableCollision();

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    bool IsMeshAssetValid() const;

    FORCEINLINE UMeshComponent* GetMesh() const
    {
        if (MeshType == EItemMeshType::StaticMesh)
        {
            return StaticMesh;
        }
        return SkeletalMesh;
    }

    FORCEINLINE FItemSlot& GetItemSlot() { return ItemSlot; }

    FORCEINLINE void SetItemSlot(const FItemSlot& InItemSlot) { ItemSlot = InItemSlot; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    EItemMeshType MeshType{EItemMeshType::None};

    UPROPERTY(EditAnywhere, Category = "Item")
    FItemSlot ItemSlot;
};