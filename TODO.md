## Bugs
- UDemoHUDWidget::UpdateLockOnPosition: 약간 오른쪽에 표시됨

## TODO
### Animation
- Assets needed: Pivot, Idle, Walk, Run, Sprint, Jump, HeavyAttack, Dodge, Hit, Death, Draw/Sheath

### Combat
CombatComponent:
- IsCombatEnabled
- Dodge, Block
- 공격 중 장비 변경 금지
- 타격감

Attack:
- CollisionComponent?
- Multiple sockets in actor -> line traces from last frame?
- HitBox? HitFrame?

### Else
UI:
- AI health bar image
- Bind: OnPossess에서 하기
- All UI: Z order (new depth = separate draw calls?)
- Focus: Page 밖의 Background 클릭 시 Page 단축키 안 먹힘
- Damage number
- HUD에 tab키 안내
- DragDropOp: ItemSlotDragSource? What happens if drag from Inventory to Equipment?
- WBP_TabButton: Button style
- Cache RowHandle in ItemInfo

ItemActionDispatcher:
- Names: Include inventory, equipment
- Too many delegates?

Data:
- 클래스마다 흩어져 있는 BPClass 찾기를 하나의 DataAsset?에 모으기?
- Class별 EditAnywhere 변수를 DataAsset?으로 처리

PlayerCharacter:
- Actions, bindings in generic way -> Map? Array?

ContextMenu:
- Lock 버튼, LockedImage
- Use/Equip, Lock/Unlock 등 context에 따른 label
- PlayerMenu나 Inventory 탭이 바뀌면 닫기

Inventory:
- AddItem: Index 지정됐고, 이미 있으면 빈 칸에 넣기 bFlag?
- DropItem: 벽 너머에 스폰됨

Equipment:
- Equip 안에서 Unequip할 때 같은 slot에 돌려놓기 (비었으면)
> DesignatedIndex?

Design:
- ContextMenu HoveredImage 겹쳐서 Hover 바뀌는 지점이 애매함
- TabButton Style: image + style? UImage + color?
- Hit damage

ESC:
- Name: GameMenu? PauseMenu? SystemMenu?
- Settings, Exit to MainMenu, Exit to Desktop

Animation:
- Idlebreak
- BlendSpace - 자연스럽게

Boss:
- Boss BT
- AIStatus in HUD
- Intro sequence?

## check
### 모든 클래스: 초기값 세팅이 필요한 변수 세팅 검사하기

### Inventory
ValidateActionRequest: Index가 -1이면 앞에서부터 찾아서 처리하기를 추가할지?

### PickUp
Box의 중심이 무기의 손잡이 등 Mesh의 Root로 가서 위치가 안 맞음.

### Optimize
- UTargetingComponent::FindTarget()

## misc
### Life cycle
- 모든 delegate.AddDynamic의 RemoveDynamic 필요 여부
- 모든 포인터의 생애 주기?

### UX
- SFX: Drop
- LockOn 대상이 벽 뒤로 가면 해제
> 앞에 잠깐 지나가는 물체에 해제되면 불편함

## Add
### Item::Tokens
Money, EXP, souls?

### 옵션 기능
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

### Audio
MusicComponent? MusicSystem in GameInstance? SoundClasses?

### Misc
- Change LockOn target
- LockOn disadvantages: Can't sprint?
- Interactable: Glowing effect
- 이펙트 + 문 등장 -> 이동
- 나침반
- 중력 이상: 캐릭터마다 중력을 다르게? (https://dev.epicgames.com/community/learning/tutorials/w6l7/unreal-engine-custom-gravity-in-ue-5-4)
- 길게 눌러 닫기
- Targeting point를 특정 Socket으로 고정
- 우편함에서 선물 수령

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
@TODO, @check, @misc, @hardcoded, @TEST, @debug 등 주석 확인 및 정리하기\
테스트 코드 제거하기

@debug -> 중요한 곳은 알림 메시지 등 게임 로직으로 처리하고, 나머지는 LogCategory 구분하여 중요한 로그만 남기기

## 시간이 된다면 공부할 부분
### GAS

### CommonUI

### Optimization
