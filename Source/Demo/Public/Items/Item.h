// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/ItemTypes.h"
#include "Interfaces/Interactable.h"
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
    // Adjust the box size to fit the mesh's bounds.
    // Not visible, but blocks visibility to be detected by line traces.
    UPROPERTY(VisibleAnywhere, Category = "Item")
    TObjectPtr<UBoxComponent> InteractCollision;

    ////////////////////////////////////////////////////////
    //        Statics
    ////////////////////////////////////////////////////////
public:
    static AItem* SpawnItem(
        UWorld* World,
        const FItemSlot& InSlot,
        const FTransform& SpawnTransform,
        bool bDisableCollision = false,
        AActor* Owner = nullptr,
        APawn* Instigator = nullptr,
        ESpawnActorCollisionHandlingMethod CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    // @param Dropper Actor who drops the item in the world.
    // @return Actually dropped quantity, -1 if failed.
    static int32 DropItem(const AActor* Dropper, const FItemSlot& InSlot);

    ////////////////////////////////////////////////////////
    //        Fundamentals
    ////////////////////////////////////////////////////////
public:
    AItem();

protected:
    virtual void BeginPlay() override;

public:
#if WITH_EDITOR
    virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

    ////////////////////////////////////////////////////////
    //        Item functions
    ////////////////////////////////////////////////////////
public:
    bool Drop(const AActor* Dropper);

    void SimulatePhysics();

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
    UPROPERTY(EditAnywhere, Category = "Initialization")
    FItemSlot ItemSlot;

    EItemMeshType MeshType{EItemMeshType::None};

private:
    static constexpr inline float DropDistance{50.f};
    static constexpr inline float DropHeight{50.f};
    static constexpr inline float DropImpulseStrength{250.f};
};