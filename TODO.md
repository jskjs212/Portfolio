## Bugs
안 보이는 사물 줍기 가능
- Visibility trace 추가?

## TODO
### UI
PlayerHUDWidget에 Interact 넣고 캔버스 줄이기?

UUIManagementSubsystem: 전역 UI
- PlayerMenu: 스탯, 장비, 인벤, 퀘스트, 맵, 업적, 설정, etc.
- MainMenu: 로비, 세션, 크레딧, 옵션, etc.
- LoadingScreen

PlayerController: 플레이어 UI
- PlayerHUD: Resource, Crosshair, Interact, QuickSlot, Minimap, etc.

BaseMenu: TabButtons, PageSwitcher, Close\
  PlayerMenu: StatsPage, EquipmentPage, InventoryPage, etc.\
  TraderMenu: BuyPage, SellPage\
StatsPage: ResourceBar, ResourceText\
EquipmentPage: ItemSlotWidget? EquipmentSlotWidget?\
InventoryPage: ItemTabSwitcher, ItemSlotWidget, Drag & Drop, ToolTip\
ContextMenu: Actions\
Button: UButton? CustomButton?

## check
### 모든 클래스: 초기값 세팅이 필요한 변수 세팅 검사하기

### Inventory
ValidateActionRequest: Index가 -1이면 앞에서부터 찾아서 처리하기를 추가할지?

### PickUp
Box의 중심이 Mesh의 Root로 가서 위치가 안 맞음.

## misc
### Life cycle
- 모든 delegate.AddDynamic의 RemoveDynamic 필요 여부
- 모든 포인터의 생애 주기?

### Inventory
- int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

### DataTable
- 현재 StaticMesh는 Collision을, SkeletalMesh는 PhysicsAsset을 수동으로 할당해줘야 함.
> Valid 체크만 할지, 없으면 자동으로 생성해줄지?

## Add
### Item::Tokens
Money, EXP, souls?

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

## 시간이 된다면 공부할 부분
### GAS

### CommonUI

### Optimization
