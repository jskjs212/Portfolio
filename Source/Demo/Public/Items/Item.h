// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Items/ItemTypes.h"
#include "Item.generated.h"

class UBoxComponent;
class UMeshComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;

UENUM()
enum class EItemMeshType : uint8
{
    None,
    StaticMesh,
    SkeletalMesh
};

UCLASS()
class DEMO_API AItem : public AActor, public IInteractable
{
    GENERATED_BODY()

    ////////////////////////////////////////////////////////
    //        Subobjects
    ////////////////////////////////////////////////////////
protected:
    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<UStaticMeshComponent> StaticMesh;

    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

    // Collision for item pickup.
    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<UBoxComponent> InteractCollision;

    ////////////////////////////////////////////////////////
    //        Statics
    ////////////////////////////////////////////////////////
public:
    // @check - What if AItem is inherited by AWeapon, AArmor, etc.?
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

#if WITH_EDITOR
    virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

    ////////////////////////////////////////////////////////
    //        Item functions
    ////////////////////////////////////////////////////////
protected:
    void SetupMesh();

    void DisableCollision();

    ////////////////////////////////////////////////////////
    //        Interactable interface
    ////////////////////////////////////////////////////////
public:
    // Add to inventory.
    virtual void Interact(APawn* InstigatorPawn) override;

    virtual void ShowHighlight(bool bShow) override;

    ////////////////////////////////////////////////////////
    //        Get & set
    ////////////////////////////////////////////////////////
public:
    bool IsMeshAssetValid() const;

    // Returns static or skeletal mesh based on the asset type.
    FORCEINLINE UMeshComponent* GetMesh() const
    {
        if (MeshType == EItemMeshType::StaticMesh)
        {
            return StaticMesh;
        }
        return SkeletalMesh;
    }

    FORCEINLINE FItemSlot& GetItemSlot() { return ItemSlot; }

    FORCEINLINE const FItemSlot& GetItemSlot() const { return ItemSlot; }

    FORCEINLINE void SetItemSlot(const FItemSlot& InItemSlot) { ItemSlot = InItemSlot; }

    ////////////////////////////////////////////////////////
    //        Variables
    ////////////////////////////////////////////////////////
protected:
    EItemMeshType MeshType{EItemMeshType::None};

    UPROPERTY(EditAnywhere, Category = "Item")
    FItemSlot ItemSlot;
};