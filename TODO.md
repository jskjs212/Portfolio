## Bugs
PCH warning

## TODO
### UI
- Bind: OnPossess에서 하기
- All UI: Z order
- Page 하위 위젯 외의 뭔가를 클릭하면 Page 포커스가 사라짐

InventoryPage:
- Drag & Drop

StateComponent?
- CharacterState, CharacterAction?

CombatComponent:
- Attack, Dodge, Block

ESC:
- Name: GameMenu? PauseMenu? SystemMenu?
- Settings, Exit to MainMenu, Exit to Desktop

BPClass:
- 클래스마다 흩어져 있는 BPClass 찾기를 하나의 DataAsset?에 모으기?

ContextMenu:
- PlayerMenu나 Inventory 탭이 바뀌면 닫기

Inventory:
- AddItem: Index 지정됐고, 이미 있으면 빈 칸에 넣기 bFlag?

Equipment:
- Equip 안에서 Unequip할 때 같은 slot에 돌려놓기 (비었으면)
> DesignatedIndex?

Design:
- ContextMenu HoveredImage 겹쳐서 Hover 바뀌는 지점이 애매함
- TabButton Style: image + style? UImage + color?

## check
### 모든 클래스: 초기값 세팅이 필요한 변수 세팅 검사하기

### Inventory
ValidateActionRequest: Index가 -1이면 앞에서부터 찾아서 처리하기를 추가할지?

### PickUp
Box의 중심이 무기의 손잡이 등 Mesh의 Root로 가서 위치가 안 맞음.

## misc
### Life cycle
- 모든 delegate.AddDynamic의 RemoveDynamic 필요 여부
- 모든 포인터의 생애 주기?

### Inventory
- int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

### DataTable
- 현재 StaticMesh는 Collision을, SkeletalMesh는 PhysicsAsset을 수동으로 할당해줘야 함.
> Valid 체크만 할지, 없으면 자동으로 생성해줄지?

### UX
- 줍기, 버리기, 장착, 사용 등 피드백이 없으니 아쉬움 -> 알림창?

## Add
### Item::Tokens
Money, EXP, souls?

### 옵션 기능
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

### Audio
MusicComponent? MusicSystem in GameInstance? SoundClasses?

### Misc
Interactable: Glowing effect

## Debug history
- Nuget warning: Just update packages...
- 25-9-12: GC issue
> 게임 중 Character, Component, UI 등 변경 없이 가만히 있어도 1분 후부터 ContextMenu의 Use, Drop이 안 먹힘. ItemActionDispatcher의 Request()에서 IsBound() == false. 생성, 바인드, 호출 등 코드 흐름 모두 이상 없음.\
> GC되던 DemoPlayerController의 ItemActionDispatcher에 UPROPERTY()를 붙여 해결.

## Final tasks
### 문서화
AnimInstance: MultiThreadUpdate
Others: Multi-threading, replication not considered.

### 정리
TODO:, check:, misc:, hardcoded:, TEST:, debug: 등 주석 확인 및 정리하기\
테스트 코드 제거하기

debug: -> 중요한 곳은 알림 메시지 등 게임 로직으로 처리하고, 나머지는 LogCategory 구분하여 중요한 로그만 남기기

## 시간이 된다면 공부할 부분
### GAS

### CommonUI

### Optimization
