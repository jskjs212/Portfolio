// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StatsComponent.h"
#include "DemoTypes/DemoGameplayTags.h"
#include "DemoTypes/ItemTypes.h"
#include "DemoTypes/TableRowBases.h"
#include "GameFramework/Pawn.h"
#include "Items/Item.h"
#include "PlayerController/DemoPlayerController.h"
#include "UI/ItemActionDispatcher.h"

DEFINE_LOG_CATEGORY(LogInventory);

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    InitInventoryComponent();
    InitMaxSlots();
    BindToItemActionDispatcher();
}

int32 UInventoryComponent::AddItem(const FItemActionRequest& Request)
{
    const int32 OriginalQuantity = Request.Slot.Quantity;
    int32 RemainingQuantity = Request.Slot.Quantity;

    // Validate and get data
    FInventoryValidationResult ValidationResult = AddItem_Validate(Request.Slot);
    if (!ValidationResult.bIsValid)
    {
        return -1; // Log in AddItem_Validate()
    }

    // Add item
    int32 MaxStackSize = ValidationResult.ItemData->MaxStackSize;
    int32 MaxSlotSize = MaxSlotSizes[ValidationResult.ItemCategory];
    bool bSuccess = AddItem_Internal(
        Request.Slot.RowHandle,
        Request.DesignatedIndex,
        MaxStackSize,
        MaxSlotSize,
        RemainingQuantity,
        *ValidationResult.ItemArray
    );
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

    UE_LOG(LogInventory, Display, TEXT("Add item - %s, %d"), *ValidationResult.ItemData->Name.ToString(), Added);
    return Added;
}

int32 UInventoryComponent::RemoveItem(const FItemActionRequest& Request)
{
    // Validate and get data
    FInventoryValidationResult ValidationResult = ValidateActionRequest(Request);
    if (!ValidationResult.bIsValid)
    {
        return -1; // Log in ValidateActionRequest()
    }

    int32 Removed = RemoveItem_Internal(ValidationResult, Request.DesignatedIndex, Request.Quantity);

    UE_LOG(LogInventory, Display, TEXT("Remove item - %s, %d"), *ValidationResult.ItemData->Name.ToString(), Removed);
    return Removed;
}

int32 UInventoryComponent::UseItem(const FItemActionRequest& Request)
{
    // Validate and get data
    FInventoryValidationResult ValidationResult = ValidateActionRequest(Request);
    if (!ValidationResult.bIsValid)
    {
        return -1; // Log in ValidateActionRequest()
    }

    const int32 ToUse = FMath::Min(Request.Quantity, ValidationResult.ItemSlot->Quantity);
    int32 Used = UseItem_Internal(*ValidationResult.ItemSlot, ValidationResult.ItemData->ItemType, ToUse);
    if (Used < 0)
    {
        return -1; // Log in UseItem_Internal()
    }

    int32 Removed = RemoveItem_Internal(ValidationResult, Request.DesignatedIndex, Used);
    checkf(Used == Removed, TEXT("Used %d, but removed %d"), Used, Removed);

    UE_LOG(LogInventory, Display, TEXT("Use item - %s, %d"), *ValidationResult.ItemData->Name.ToString(), Removed);
    return Removed;
}

int32 UInventoryComponent::DropItem(const FItemActionRequest& Request)
{
    // Validate and get data
    FInventoryValidationResult ValidationResult = ValidateActionRequest(Request);
    if (!ValidationResult.bIsValid)
    {
        return -1; // Log in ValidateActionRequest()
    }

    const int32 ToDrop = FMath::Min(Request.Quantity, ValidationResult.ItemSlot->Quantity);
    const FItemSlot DropSlot = FItemSlot{ValidationResult.ItemSlot->RowHandle, ToDrop, false};
    int32 Dropped = AItem::DropItem(GetWorld(), DropSlot, GetOwner());
    if (Dropped < 0)
    {
        return -1; // Log in AItem::DropItem()
    }

    int32 Removed = RemoveItem_Internal(ValidationResult, Request.DesignatedIndex, Dropped);
    checkf(Dropped == Removed, TEXT("Dropped %d, but removed %d"), Dropped, Removed);

    UE_LOG(LogInventory, Display, TEXT("Drop item - %s, %d"), *ValidationResult.ItemData->Name.ToString(), Removed);
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

bool UInventoryComponent::AddMaxSlotSize(FGameplayTag ItemCategory, const int32 ToAdd)
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

void UInventoryComponent::InitInventoryComponent()
{
    // Inventory setup for fixed item categories.
    for (const FGameplayTag& ItemCategory : DemoItemTypes::GetItemCategories())
    {
        OwnedItems.Add(ItemCategory, FItemArray{});
        MaxSlotSizes.Add(ItemCategory, 0);
    }

    checkf(OwnedItems.Num() == DemoItemTypes::GetItemCategories().Num(), TEXT("OwnedItems should have all ItemCategories."));
    checkf(MaxSlotSizes.Num() == DemoItemTypes::GetItemCategories().Num(), TEXT("MaxSlotSizes should have all ItemCategories."));
}

void UInventoryComponent::InitMaxSlots()
{
    // @TODO - Config file or data table? Determined by character class or something?
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

    // Inventory page is not initialized yet, but just in case.
    OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::BindToItemActionDispatcher()
{
    if (APawn* Pawn = Cast<APawn>(GetOwner()))
    {
        if (ADemoPlayerController* DemoPlayerController = Pawn->GetController<ADemoPlayerController>())
        {
            if (UItemActionDispatcher* ItemActionDispatcher = DemoPlayerController->GetItemActionDispatcher())
            {
                ItemActionDispatcher->OnAddItemRequested.BindUObject(this, &ThisClass::AddItem);
                ItemActionDispatcher->OnRemoveItemRequested.BindUObject(this, &ThisClass::RemoveItem);
                ItemActionDispatcher->OnUseItemRequested.BindUObject(this, &ThisClass::UseItem);
                ItemActionDispatcher->OnDropItemRequested.BindUObject(this, &ThisClass::DropItem);
                ItemActionDispatcher->OnSwapItemRequested.BindUObject(this, &ThisClass::SwapItem);
            }
        }
    }
}

FInventoryValidationResult UInventoryComponent::AddItem_Validate(const FItemSlot& InSlot)
{
    if (!InSlot.IsValid())
    {
        UE_LOG(LogInventory, Warning, TEXT("AddItem() - Slot is not valid."));
        return {};
    }

    if (InSlot.bIsLocked)
    {
        UE_LOG(LogInventory, Warning, TEXT("AddItem() - Slot is locked."));
        return {};
    }

    // Get ItemData
    const FItemDataBase* ItemData = InSlot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryComponent::AddItem_Validate()"));
    if (!ItemData)
    {
        return {}; // Log in GetRow()
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
        return {};
    }

    FInventoryValidationResult Result;
    Result.bIsValid = true;
    Result.ItemCategory = ItemCategory;
    Result.ItemData = ItemData;
    Result.ItemArray = ItemArray;

    return Result;
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
            ItemArray.Emplace(InSlotRowHandle, ToAdd);
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

FInventoryValidationResult UInventoryComponent::ValidateActionRequest(const FItemActionRequest& Request)
{
    if (Request.Quantity <= 0)
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - Quantity is not valid."));
        return {};
    }

    const FItemDataBase* ItemData = Request.Slot.RowHandle.GetRow<FItemDataBase>(TEXT("UInventoryComponent::ValidateActionRequest()"));
    if (!ItemData)
    {
        return {}; // Log in GetRow()
    }

    FGameplayTag ItemCategory = DemoItemTypes::GetItemCategory(ItemData->ItemType);

    // Category is validated in data table.
    TArray<FItemSlot>& ItemArray = OwnedItems[ItemCategory].ItemArray;
    if (Request.DesignatedIndex < 0 || Request.DesignatedIndex >= ItemArray.Num())
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - DesignatedIndex is out of range."));
        return {};
    }

    FItemSlot& Slot = ItemArray[Request.DesignatedIndex];
    if (IsInventorySlotEmpty(Slot) || Slot.RowHandle != Request.Slot.RowHandle)
    {
        UE_LOG(LogInventory, Error, TEXT("ValidateActionRequest() - Item at DesignatedIndex is different. This request is not from user input."));
        return {};
    }

    if (Slot.bIsLocked)
    {
        UE_LOG(LogInventory, Warning, TEXT("ValidateActionRequest() - Slot is locked."));
        return {};
    }

    FInventoryValidationResult Result;
    Result.bIsValid = true;
    Result.ItemCategory = ItemCategory;
    Result.ItemSlot = &Slot;
    Result.ItemData = ItemData;
    Result.ItemArray = &ItemArray;

    return Result;
}

int32 UInventoryComponent::RemoveItem_Internal(FInventoryValidationResult& InOutValidationResult, const int32 ValidatedIndex, const int32 Quantity)
{
    // Remove item
    const int32 Removed = FMath::Min(Quantity, InOutValidationResult.ItemSlot->Quantity);
    InOutValidationResult.ItemSlot->Quantity -= Removed;

    // Remove empty slot if needed
    if (!bFixSlotSizeAndExposeEmptySlots && InOutValidationResult.ItemSlot->Quantity == 0)
    {
        InOutValidationResult.ItemArray->RemoveAt(ValidatedIndex);
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