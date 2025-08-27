## Bugs
### ItemDataBase -> WeaponData, ArmorData
상속된 Data를 바탕으로 DataTable은 생성되는데 static_cast가 하나만 정확히 고르지 못함 -> UObject와 연동할 수 있을까?

(Weapon인데 Armor casting도 != nullptr)

## TODO
### AddItem
bAllowMultipleSlots = false이고 이미 있을 때 구체적인 알림

## check
### 모든 클래스: 초기값 세팅이 필요한 변수 세팅 검사하기

## misc
### Life cycle
- 모든 delegate.AddDynamic의 RemoveDynamic 필요 여부
- 모든 포인터의 생애 주기?

### Inventory
int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

## Add
### 옵션 기능
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

## Solved?

## Final tasks
### 문서화
AnimInstance: MultiThreadUpdate
Others: Multi-threading, replication not considered.

### 정리
TODO:, check:, misc:, TEST: 등 주석 확인 및 정리하기\
테스트 코드 제거하기

Log -> 중요한 곳은 알림 메시지 등 게임 로직으로 처리하고, 나머지는 LogCategory 구분하여 로그 남기기

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