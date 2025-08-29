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
- Call by value할 때 헤더에 const가 없어도 되지만, body에서 수정할 계획이 없다면 cpp에서만 const 선언하기

### Item
- Inventory, Equipment의 함수는 자신이 소유한 ItemSlot만을 수정한다.
> 외부에서 AddItem, EquipItem 등에 자신의 ItemSlot을 const로 넘긴 후 필요시 함수 리턴값 만큼 직접 수정해야 한다.