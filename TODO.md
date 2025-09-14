## Bugs
PCH warning

## TODO
### Combat
StateManager:
- State transition (Jump, ...)
- Jump testing

HandleDeath()

CombatComponent:
- Attack, Dodge, Block

### UI
- Bind: OnPossess���� �ϱ�
- All UI: Z order
- Page ���� ���� ���� ������ Ŭ���ϸ� Page ��Ŀ���� �����

BPClass:
- Ŭ�������� ����� �ִ� BPClass ã�⸦ �ϳ��� DataAsset?�� ������?

ContextMenu:
- Lock ��ư, LockedImage
- Use/Equip, Lock/Unlock �� context�� ���� label
- PlayerMenu�� Inventory ���� �ٲ�� �ݱ�

Inventory:
- AddItem: Index �����ư�, �̹� ������ �� ĭ�� �ֱ� bFlag?

Equipment:
- Equip �ȿ��� Unequip�� �� ���� slot�� �������� (�������)
> DesignatedIndex?

Design:
- ContextMenu HoveredImage ���ļ� Hover �ٲ�� ������ �ָ���
- TabButton Style: image + style? UImage + color?

ESC:
- Name: GameMenu? PauseMenu? SystemMenu?
- Settings, Exit to MainMenu, Exit to Desktop

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

### DataTable
- ���� StaticMesh�� Collision��, SkeletalMesh�� PhysicsAsset�� �������� �Ҵ������ ��.
> Valid üũ�� ����, ������ �ڵ����� ����������?

### UX
- SFX: Drop

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
> ���� �� Character, Component, UI �� ���� ���� ������ �־ 1�� �ĺ��� ContextMenu�� Use, Drop�� �� ����. ItemActionDispatcher�� Request()���� IsBound() == false. ����, ���ε�, ȣ�� �� �ڵ� �帧 ��� �̻� ����.\
> GC�Ǵ� DemoPlayerController�� ItemActionDispatcher�� UPROPERTY()�� �ٿ� �ذ�.

## Final tasks
### ����ȭ
AnimInstance: MultiThreadUpdate
Others: Multi-threading, replication not considered.

### ����
@TODO, @check, @misc, @hardcoded, @TEST, @debug �� �ּ� Ȯ�� �� �����ϱ�\
�׽�Ʈ �ڵ� �����ϱ�

@debug -> �߿��� ���� �˸� �޽��� �� ���� �������� ó���ϰ�, �������� LogCategory �����Ͽ� �߿��� �α׸� �����

## �ð��� �ȴٸ� ������ �κ�
### GAS

### CommonUI

### Optimization
