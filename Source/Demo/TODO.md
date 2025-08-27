## Bugs
### ItemDataBase -> WeaponData, ArmorData
��ӵ� Data�� �������� DataTable�� �����Ǵµ� static_cast�� �ϳ��� ��Ȯ�� ���� ���� -> UObject�� ������ �� ������?

(Weapon�ε� Armor casting�� != nullptr)

## TODO
### AddItem
bAllowMultipleSlots = false�̰� �̹� ���� �� ��ü���� �˸�

## check
### ��� Ŭ����: �ʱⰪ ������ �ʿ��� ���� ���� �˻��ϱ�

## misc
### Life cycle
- ��� delegate.AddDynamic�� RemoveDynamic �ʿ� ����
- ��� �������� ���� �ֱ�?

### Inventory
int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

## Add
### �ɼ� ���
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

## Solved?

## Final tasks
### ����ȭ
AnimInstance: MultiThreadUpdate
Others: Multi-threading, replication not considered.

### ����
TODO:, check:, misc:, TEST: �� �ּ� Ȯ�� �� �����ϱ�\
�׽�Ʈ �ڵ� �����ϱ�

Log -> �߿��� ���� �˸� �޽��� �� ���� �������� ó���ϰ�, �������� LogCategory �����Ͽ� �α� �����

## Conventions
### Header Ordering
1. License
1. Includes
1. Log categories
1. Forward declarations
1. Other classes' definitions (UENUM, USTRUCT, etc.)
1. Delegates' definitions
1. Main class's definition
1. Template specializations

### Class Member Ordering
1. Friends
1. Typedefs
1. Subobjects (components, etc.)
1. Static members
1. Delegates
1. Fundamentals (constructor, BeginPlay, Tick, etc.)
1. Class specific functions
1. Getters
1. Setters
1. Variables (simple -> complex)
> Core members (Items in Inventory) could be first.\
> Each group: bool -> int -> float -> enum -> structs -> pointer references\
> (Strict rule or alignment < readability)