## Bugs
- Page ���� ���� ���� ������ Ŭ���ϸ� Page ��Ŀ���� �����
- ContextMenu ��ư ���ļ� Hover �ٲ�� ������ �ָ���

## TODO
### UI
- �ǹ�ư image + style? UImage + color?
- All UI: Z order

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
- Ŭ�������� ����� �ִ� BPClass ã�⸦ �ϳ��� DataAsset?�� ������?

ContextMenu:
- PlayerMenu�� Inventory ���� �ٲ�� �ݱ�

Equipment:
- Equip �ȿ��� Unequip�� �� ���� slot�� �������� (�������)

- OnPossess: Bind to UI

## check
### ��� Ŭ����: �ʱⰪ ������ �ʿ��� ���� ���� �˻��ϱ�

### Inventory
ValidateActionRequest: Index�� -1�̸� �տ������� ã�Ƽ� ó���ϱ⸦ �߰�����?

### PickUp
Box�� �߽��� ������ ������ �� Mesh�� Root�� ���� ��ġ�� �� ����.

## misc
### Life cycle
- ��� delegate.AddDynamic�� RemoveDynamic �ʿ� ����
- ��� �������� ���� �ֱ�?

### Inventory
- int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

### DataTable
- ���� StaticMesh�� Collision��, SkeletalMesh�� PhysicsAsset�� �������� �Ҵ������ ��.
> Valid üũ�� ����, ������ �ڵ����� ����������?

### UX
- �ݱ�, ������, ����, ��� �� �ǵ���� ������ �ƽ��� -> �˸�â?

## Add
### Item::Tokens
Money, EXP, souls?

### �ɼ� ���
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

### Audio
MusicComponent? MusicSystem in GameInstance? SoundClasses?

### Misc
Interactable: Glowing effect

## Debug history
- Nuget warning: Just update packages...
- 25-9-12: GC issue
> ���� �� Character, Component, UI �� ���� ���� ������ �־ 1�� �ĺ��� ContextMenu�� Use, Drop�� �� ����. ItemActionDispatcher�� Request()���� IsBound() == false, null ���� �߻�. ����, ���ε�, ȣ�� �� �ڵ� �帧 ��� �̻� ����.\
> DemoPlayerController�� ItemActionDispatcher�� UPROPERTY()�� ���̴� ������ �ذ�.\
> UPROPERTY() ���÷����� ��� CreateDefaultSubobject�� ���� �� Controller ���� Mark�� ���� �ʾ� GC ����� �Ǿ��� ��.

## Final tasks
### ����ȭ
AnimInstance: MultiThreadUpdate
Others: Multi-threading, replication not considered.

### ����
TODO:, check:, misc:, hardcoded:, TEST:, debug: �� �ּ� Ȯ�� �� �����ϱ�\
�׽�Ʈ �ڵ� �����ϱ�

debug: -> �߿��� ���� �˸� �޽��� �� ���� �������� ó���ϰ�, �������� LogCategory �����Ͽ� �߿��� �α׸� �����

## �ð��� �ȴٸ� ������ �κ�
### GAS

### CommonUI

### Optimization
