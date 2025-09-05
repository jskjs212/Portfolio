// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/TableRowBases.h"
#include "Items/Item.h"
#include "Items/ItemTypes.h"

DEFINE_LOG_CATEGORY(LogInventory);

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // Inventory setup for fixed item categories.
    for (const FGameplayTag& ItemCategory : DemoItemTypes::GetItemCategories())
    {
        OwnedItems.Add(ItemCategory, FItemArray{});
        MaxSlotSizes.Add(ItemCategory, 0);
    }

    checkf(OwnedItems.Num() == DemoItemTypes::GetItemCategories().Num(), TEXT("OwnedItems should have all ItemCategories."));
    checkf(MaxSlotSizes.Num() == DemoItemTypes::GetItemCategories().Num(), TEXT("MaxSlotSizes should have all ItemCategories."));

    InitMaxSlots();
}

int32 UInventoryComponent::AddItem(const FItemSlot& InSlot, const int32 DesignatedIndex)
{
    const int32 OriginalQuantity = InSlot.Quantity;
    int32 RemainingQuantity = InSlot.Quantity;
    FInventoryValidatedData ValidatedData;

    // Validate and get data
    bool bValid = AddItem_Validate(InSlot, ValidatedData);
    if (!bValid)
    {
        return -1; // Log in AddItem_Validate()
    }

    // Add item
    int32 MaxStackSize = ValidatedData.ItemData->MaxStackSize;
    int32 MaxSlotSize = MaxSlotSizes[ValidatedData.ItemCategory];
    bool bSuccess = AddItem_Internal(InSlot.RowHandle, DesignatedIndex, MaxStackSize, MaxSlotSize, RemainingQuantity, *ValidatedData.ItemArray);
    if (!bSuccess)
    {
        return -1; // Log in AddItem_Internal()
    }

    // Broadcast if added
    const int32 Added = OriginalQuantity - RemainingQuantity;
    if (Added > 0)
    {
        OnInventoryUpdated.Broadcast();
    }

    UE_LOG(LogInventory, Display, TEXT("Add item - %s, %d"), *ValidatedData.ItemData->Name.ToString(), Added);
    return Added;
}

int32 UInventoryComponent::RemoveItem(const FItemActionRequest& Request)
{
    // Validate and get data
    FInventoryValidatedData ValidatedData;
    bool bValid = ValidateActionRequest(Request, ValidatedData);
    if (!bValid)
    {
        return -1; // Log in ValidateActionRequest()
    }

    int32 ToRemove = RemoveItem_Internal(ValidatedData, Request.DesignatedIndex, Request.Quantity);

    UE_LOG(LogInventory, Display, TEXT("Remove item - %s, %d"), *ValidatedData.ItemData->Name.ToString(), ToRemove);
    return ToRemove;
}

int32 UInventoryComponent::UseItem(const FItemActionRequest& Request)
{
    // Validate and get data
    FInventoryValidatedData ValidatedData;
    bool bValid = ValidateActionRequest(Request, ValidatedData);
    if (!bValid)
    {
        return -1; // Log in ValidateActionRequest()
    }

    const int32 ToUse = FMath::Min(Request.Quantity, ValidatedData.ItemSlot->Quantity);
    int32 Used = UseItem_Internal(*ValidatedData.ItemSlot, ValidatedData.ItemData->ItemType, ToUse);
    if (Used < 0)
    {
        return -1; // Log in UseItem_Internal()
    }

    int32 Removed = RemoveItem_Internal(ValidatedData, Request.DesignatedIndex, Used);
    checkf(Used == Removed, TEXT("Used %d, but removed %d"), Used, Removed);

    UE_LOG(LogInventory, Display, TEXT("Use item - %s, %d"), *ValidatedData.ItemData->Name.ToString(), Removed);
    return Removed;
}

int32 UInventoryComponent::DropItem(const FItemActionRequest& Request)
{
    // Validate and get data
    FInventoryValidatedData ValidatedData;
    bool bValid = ValidateActionRequest(Request, ValidatedData);
    if (!bValid)
    {
        return -1; // Log in ValidateActionRequest()
    }

    const int32 ToDrop = FMath::Min(Request.Quantity, ValidatedData.ItemSlot->Quantity);
    int32 Dropped = DropItem_Internal(*ValidatedData.ItemSlot, ToDrop);
    if (Dropped < 0)
    {
        return -1; // Log in DropItem_Internal()
    }

    int32 Removed = RemoveItem_Internal(ValidatedData, Request.DesignatedIndex, Dropped);
    checkf(Dropped == Removed, TEXT("Dropped %d, but removed %d"), Dropped, Removed);

    UE_LOG(LogInventory, Display, TEXT("Drop item - %s, %d"), *ValidatedData.ItemData->Name.ToString(), Removed);
    return Removed;
}

bool UInventoryComponent::SwapItem(const FGameplayTag ItemCategory, const int32 FirstIndex, const int32 SecondIndex)
{
    if (FirstIndex == SecondIndex)
    {
        UE_LOG(LogInventory, Display, TEXT("SwapItem() - Same index."));
        return false;
    }

    FItemArray* ItemArrayPtr = OwnedItems.Find(ItemCategory);
    if (!ItemArrayPtr)
    {
        UE_LOG(LogInventory, Error, TEXT("SwapItem() - ItemCategory is not valid."));
        return false;
    }

    TArray<FItemSlot>& ItemArray = ItemArrayPtr->ItemArray;
    if (!ItemArray.IsValidIndex(FirstIndex) || !ItemArray.IsValidIndex(SecondIndex))
    {
        UE_LOG(LogInventory, Warning, TEXT("SwapItem() - Out of range."));
        return false;
    }

    const FItemSlot& FirstSlot = ItemArray[FirstIndex];
    const FItemSlot& SecondSlot = ItemArray[SecondIndex];
    if (FirstSlot.bIsLocked || SecondSlot.bIsLocked)
    {
        UE_LOG(LogInventory, Display, TEXT("SwapItem() - Slot is locked."));
        return false;
    }

    ItemArray.Swap(FirstIndex, SecondIndex);
    OnInventoryUpdated.Broadcast();
    return true;
}

bool UInventoryComponent::AddMaxSlotSize(FGameplayTag ItemCategory, int32 ToAdd)
{
    int32* MaxSlotSizePtr = MaxSlotSizes.Find(ItemCategory);
    if (!MaxSlotSizePtr)
    {
        UE_LOG(LogInventory, Error, TEXT("AddMaxSlotSize() - ItemCategory is not valid."));
        return false;
    }

    if (ToAdd <= 0 || *MaxSlotSizePtr + ToAdd > MaxAllowedSlotSize)
    {
        UE_LOG(LogInventory, Warning, TEXT("AddMaxSlotSize() - ToAdd is not valid."));
        return false;
    }

    *MaxSlotSizePtr += ToAdd;

    if (bFixSlotSizeAndExposeEmptySlots)
    {
        TArray<FItemSlot>& ItemArray = OwnedItems[ItemCategory].ItemArray;
        check(ItemArray.Num() < *MaxSlotSizePtr);
        ItemArray.SetNum(*MaxSlotSizePtr);
        OnInventoryUpdated.Broadcast();
    }

    return true;
}

void UInventoryComponent::InitMaxSlots()
{
    // TODO: Config file or data table? Determined by character class or something?
    // Initial max slot sizes.
    constexpr int32 WeaponMaxSlots = 10;
    constexpr int32 ArmorMaxSlots = 10;
    constexpr int32 ConsumableMaxSlots = 20;
    MaxSlotSizes[DemoGameplayTags::Item_Weapon] = WeaponMaxSlots;
    MaxSlotSizes[DemoGameplayTags::Item_Armor] = ArmorMaxSlots;
    MaxSlotSizes[DemoGameplayTags::Item_Consumable] = ConsumableMaxSlots;

    // Fill with empty slots if needed.
    if (bFixSlotSizeAndExposeEmptySlots)
    {
        for (TPair<FGameplayTag, FItemArray>& Pair : OwnedItems)
        {
            Pair.Value.ItemArray.SetNum(MaxSlotSizes[Pair.Key]);
        }
    }
}

bool UInventoryComponent::AddItem_Validate(const FItemSlot& InSlot, FInventoryValidatedData& OutData)
{
    OutData = FInventoryValidatedData{};

    if (!InSlot.IsValid())
    {
        UE_LOG(LogInventory, Warning, TEXT("AddItem() - Slot is not valid."));
        return false;
    }

    if (InSlot.bIsLocked)
    {
        UE_LOG(LogInventory, Warning, TEXT("AddItem() - Slot is locked."));
        return false;
    }

    // Get ItemData
    const FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryComponent::AddItem_Validate()"));
    if (!ItemData)
    {
        return false; // Log in GetRow()
    }

    // Find ItemType & ItemArray
    // ex) Item.Weapon tag from Item.Weapon.Melee.OneHanded tag
    FGameplayTag ItemCategory = DemoItemTypes::GetItemCategory(ItemData->ItemType);
    TArray<FItemSlot>* ItemArray = nullptr;
    if (ItemCategory.IsValid())
    {
        ItemArray = &OwnedItems[ItemCategory].ItemArray;
    }
    if (!ItemArray)
    {
        UE_LOG(LogInventory, Error, TEXT("AddItem() - ItemType is not valid."));
        return false;
    }

    OutData.ItemCategory = ItemCategory;
    OutData.ItemData = ItemData;
    OutData.ItemArray = ItemArray;
    return true;
}

bool UInventoryComponent::AddItem_Internal(
    const FDataTableRowHandle& InSlotRowHandle,
    const int32 DesignatedIndex,
    const int32 MaxStackSize,
    const int32 MaxSlotSize,
    int32& InOutRemainingQuantity,
    TArray<FItemSlot>& ItemArray
)
{
    if (DesignatedIndex < 0)
    {
        return AddItem_AutoPlacement(InSlotRowHandle, MaxStackSize, MaxSlotSize, InOutRemainingQuantity, ItemArray);
    }
    else if (DesignatedIndex < ItemArray.Num())
    {
        return AddItem_ToDesignatedSlot(InSlotRowHandle, DesignatedIndex, MaxStackSize, InOutRemainingQuantity, ItemArray);
    }
    else
    {
        UE_LOG(LogInventory, Error, TEXT("AddItem() - DesignatedIndex is out of range."));
        return false;
    }
}

bool UInventoryComponent::AddItem_AutoPlacement(
    const FDataTableRowHandle& InSlotRowHandle,
    const int32 MaxStackSize,
    const int32 MaxSlotSize,
    int32& InOutRemainingQuantity,
    TArray<FItemSlot>& ItemArray
)
{
    bool bSlotExists = false;

    // Top-up existing slots with same item
    for (FItemSlot& Slot : ItemArray)
    {
        if (!IsInventorySlotEmpty(Slot) && Slot.RowHandle == InSlotRowHandle)
        {
            // There should be 0 or 1 existing slot if multiple slots are not allowed.
            check(bAllowMultipleSlots || !bSlotExists);

            const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize - Slot.Quantity);
            bSlotExists = true;
            Slot.Quantity += ToAdd;
            InOutRemainingQuantity -= ToAdd;
        }
    }

    // Add into new slots
    int32 EmptySlotIndex = 0;
    while (InOutRemainingQuantity > 0)
    {
        // allow - continue
        // !allow && exists - break
        // !allow && !exists - continue
        if (!bAllowMultipleSlots && bSlotExists)
        {
            UE_LOG(LogInventory, Display, TEXT("AddItem() - Multiple slots are not allowed."));
            break;
        }

        // Add a new slot
        if (ItemArray.Num() < MaxSlotSize)
        {
            const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize);
            bSlotExists = true;
            ItemArray.Add(FItemSlot{InSlotRowHandle, ToAdd});
            InOutRemainingQuantity -= ToAdd;
        }
        // Fill an empty slot
        else if (bFixSlotSizeAndExposeEmptySlots)
        {
            // Find an empty slot
            while (EmptySlotIndex < ItemArray.Num() && !IsInventorySlotEmpty(ItemArray[EmptySlotIndex]))
            {
                ++EmptySlotIndex;
            }

            if (EmptySlotIndex < ItemArray.Num())
            {
                const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize);
                bSlotExists = true;
                ItemArray[EmptySlotIndex] = FItemSlot{InSlotRowHandle, ToAdd};
                InOutRemainingQuantity -= ToAdd;
            }
            else // Out of empty slots
            {
                UE_LOG(LogInventory, Display, TEXT("AddItem() - Inventory is full."));
                break;
            }
        }
        else if (ItemArray.Num() == MaxSlotSize)
        {
            UE_LOG(LogInventory, Display, TEXT("AddItem() - Inventory is full."));
            break;
        }
        else
        {
            checkNoEntry();
            break;
        }
    } // end of while (InOutRemainingQuantity > 0)

    return true;
}

bool UInventoryComponent::AddItem_ToDesignatedSlot(
    const FDataTableRowHandle& InSlotRowHandle,
    const int32 DesignatedIndex,
    const int32 MaxStackSize,
    int32& InOutRemainingQuantity,
    TArray<FItemSlot>& ItemArray
)
{
    if (!bAllowMultipleSlots)
    {
        // Check existing slots.
        // Cache if planning to use large arrays.
        for (int32 Index = 0; const FItemSlot& Slot : ItemArray)
        {
            if (DesignatedIndex != Index && !IsInventorySlotEmpty(Slot) && Slot.RowHandle == InSlotRowHandle)
            {
                UE_LOG(LogInventory, Display, TEXT("AddItem() - Multiple slots are not allowed."));
                return false;
            }
            ++Index;
        }
    }

    // Empty or same item
    FItemSlot& Slot = ItemArray[DesignatedIndex];
    if (IsInventorySlotEmpty(Slot) || Slot.RowHandle == InSlotRowHandle)
    {
        const int32 ToAdd = FMath::Min(InOutRemainingQuantity, MaxStackSize - Slot.Quantity);
        Slot.RowHandle = InSlotRowHandle;
        Slot.Quantity += ToAdd;
        InOutRemainingQuantity -= ToAdd;
    }
    else // Different item -> cancel
    {
        UE_LOG(LogInventory, Display, TEXT("AddItem() - Different item exists at designated index."));
        return false;
    }

    return true;
}

bool UInventoryComponent::ValidateActionRequest(const FItemActionRequest& Request, FInventoryValidatedData& OutData)
{
    OutData = FInventoryValidatedData{};

    if (Request.Quantity <= 0)
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - Quantity is not valid."));
        return false;
    }

    const FItemDataBase* ItemData = Request.Slot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryComponent::ValidateActionRequest()"));
    if (!ItemData)
    {
        return false; // Log in GetRow()
    }

    FGameplayTag ItemCategory = DemoItemTypes::GetItemCategory(ItemData->ItemType);

    // Category is validated in data table.
    TArray<FItemSlot>& ItemArray = OwnedItems[ItemCategory].ItemArray;
    if (Request.DesignatedIndex < 0 || Request.DesignatedIndex >= ItemArray.Num())
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - DesignatedIndex is out of range."));
        return false;
    }

    FItemSlot& Slot = ItemArray[Request.DesignatedIndex];
    if (IsInventorySlotEmpty(Slot) || Slot.RowHandle != Request.Slot.RowHandle)
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - Item at DesignatedIndex is different. This request is not from user input."));
        return false;
    }

    if (Slot.bIsLocked)
    {
        UE_LOG(LogInventory, Warning, TEXT("ValidateActionRequest() - Slot is locked."));
        return false;
    }

    OutData.ItemCategory = ItemCategory;
    OutData.ItemSlot = &Slot;
    OutData.ItemData = ItemData;
    OutData.ItemArray = &ItemArray;
    return true;
}

int32 UInventoryComponent::RemoveItem_Internal(FInventoryValidatedData& ValidatedData, const int32 ValidatedIndex, const int32 Quantity)
{
    // Remove item
    const int32 Removed = FMath::Min(Quantity, ValidatedData.ItemSlot->Quantity);
    ValidatedData.ItemSlot->Quantity -= Removed;

    // Remove empty slot if needed
    if (!bFixSlotSizeAndExposeEmptySlots && ValidatedData.ItemSlot->Quantity == 0)
    {
        ValidatedData.ItemArray->RemoveAt(ValidatedIndex);
    }

    // Broadcast if removed
    if (Removed > 0)
    {
        OnInventoryUpdated.Broadcast();
    }

    return Removed;
}

int32 UInventoryComponent::UseItem_Internal(const FItemSlot& InSlot, const FGameplayTag ItemType, const int32 Quantity)
{
    // Equipment types
    FGameplayTag EquipmentType = DemoItemTypes::GetEquipmentType(ItemType);
    if (EquipmentType.IsValid())
    {
        // Equip item
        UEquipmentComponent* EquipmentComp = GetOwner()->FindComponentByClass<UEquipmentComponent>();
        checkf(EquipmentComp, TEXT("UseItem() - Equip action was requested, but EquipmentComponent is not found."));
        return EquipmentComp->EquipItem(InSlot) ? 1 : -1;
    }
    // Consumable types
    else if (ItemType.MatchesTag(DemoGameplayTags::Item_Consumable))
    {
        return UseItem_Consume(InSlot, ItemType, Quantity);
    }

    UE_LOG(LogInventory, Error, TEXT("UseItem() - ItemType %s is not supported."), *ItemType.ToString());
    return -1;
}

int32 UInventoryComponent::UseItem_Consume(const FItemSlot& InSlot, const FGameplayTag ItemType, const int32 Quantity)
{
    const FConsumableData* ConsumableData = InSlot.RowHandle.GetRow<FConsumableData>(TEXT("UInventoryComponent::UseItem_Consume()"));
    if (!ConsumableData)
    {
        return -1; // Log in GetRow()
    }

    // Handle specific types
    if (ItemType.MatchesTag(DemoGameplayTags::Item_Consumable_Food))
    {
        return ConsumeFood(ConsumableData, Quantity);
    }
    // Potion
    // Scroll
    // Grenade
    // etc.

    UE_LOG(LogInventory, Error, TEXT("UseItem_Consume() - ItemType %s is not supported."), *ItemType.ToString());
    return -1;
}

int32 UInventoryComponent::ConsumeFood(const FConsumableData* ConsumableData, const int32 Quantity)
{
    UStatsComponent* StatsComp = GetOwner()->FindComponentByClass<UStatsComponent>();
    checkf(StatsComp, TEXT("ConsumeFood() - StatsComponent is not found."));

    // Just heal for now.
    // If health is 50/100 and HealAmount is 30, use 2 to fully heal.
    const float CurrentHealth = StatsComp->GetCurrentHealth();
    const float MaxHealth = StatsComp->GetMaxHealth();
    const int32 ToUse = FMath::Min(Quantity, FMath::CeilToInt32((MaxHealth - CurrentHealth) / ConsumableData->HealAmount));

    StatsComp->Heal(ToUse * ConsumableData->HealAmount);
    return ToUse;
}

int32 UInventoryComponent::DropItem_Internal(const FItemSlot& InSlot, const int32 Quantity)
{
    UWorld* World = GetWorld();
    AActor* OwnerActor = GetOwner();
    if (!World || !OwnerActor)
    {
        return -1;
    }

    // Spawn location
    const FVector LocationOffset = OwnerActor->GetActorForwardVector() * DropDistance + FVector{0.f, 0.f, DropHeight};
    FTransform SpawnTransform = OwnerActor->GetActorTransform();
    SpawnTransform.AddToTranslation(LocationOffset);

    // Spawn
    AItem* DroppedItem = AItem::SpawnItem(World, FItemSlot{InSlot.RowHandle, Quantity}, SpawnTransform);
    if (!IsValid(DroppedItem))
    {
        UE_LOG(LogInventory, Error, TEXT("DropItem() - Failed to spawn item."));
        return -1;
    }

    // Check mesh to drop
    if (!DroppedItem->IsMeshAssetValid())
    {
        UE_LOG(LogInventory, Error, TEXT("DropItem() - Item has no mesh."));
        DroppedItem->Destroy();
        return -1;
    }

    // Throw
    if (UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(DroppedItem->GetMesh()))
    {
        PrimitiveComp->AddImpulse(OwnerActor->GetActorForwardVector() * DropImpulseStrength, NAME_None, true);
    }

    return Quantity;
}