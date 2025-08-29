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

## Rules
### General
- Call by value�� �� ����� const�� ��� ������, body���� ������ ��ȹ�� ���ٸ� cpp������ const �����ϱ�

### Item
- Inventory, Equipment�� �Լ��� �ڽ��� ������ ItemSlot���� �����Ѵ�.
> �ܺο��� AddItem, EquipItem � �ڽ��� ItemSlot�� const�� �ѱ� �� �ʿ�� �Լ� ���ϰ� ��ŭ ���� �����ؾ� �Ѵ�.