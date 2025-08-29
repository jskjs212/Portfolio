## Bugs

## TODO
### PickUp
IPickUpable to AItem?

### UI
Button: UButton? CustomButton?
MainMenu: Stats, Equipment, Inventory, etc.
Stats: ResourceBar, ResourceText
Equipment: ItemSlotWidget?
Inventory: ItemTab, ItemSlotWidget, Drag & Drop, ToolTip
ContextMenu: Actions

## check
### 모든 클래스: 초기값 세팅이 필요한 변수 세팅 검사하기

### 모호한 함수 인자 명확히 하기
Function(true) -> Function(ESomeOption::Good)
> 1개는 괜찮은데, 인자가 많으면 헷갈림

### Inventory
- ValidateActionRequest: Index가 -1이면 앞에서부터 찾아서 처리하기를 추가할지?

## misc
### Life cycle
- 모든 delegate.AddDynamic의 RemoveDynamic 필요 여부
- 모든 포인터의 생애 주기?

### Inventory
- int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

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

Log -> 중요한 곳은 알림 메시지 등 게임 로직으로 처리하고, 나머지는 LogCategory 구분하여 중요한 로그만 남기기