## Bugs

## TODO
### ItemActionRequest, ValidateRequest, RemoveItem

## check
### ��� Ŭ����: �ʱⰪ ������ �ʿ��� ���� ���� �˻��ϱ�

## misc
### AddItem result
int32 -> EAddItemResult???

## Add

### �ɼ� ���
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

## Solved?

## Final tasks
### ����ȭ

### ����
TODO:, check:, misc:, TEST: �� �ּ� Ȯ�� �� �����ϱ�\
�׽�Ʈ �ڵ� �����ϱ�\
LogTemp -> �߿��� ���� ���� �������� ó���ϰ�, �������� LogCategory �����Ͽ� �α� �����

## Conventions
### Header Ordering
1. License
1. Includes
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