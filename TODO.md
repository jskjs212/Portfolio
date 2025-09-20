## Bugs
- PCH warning

## TODO
### Common
- In-game notifications: Low health, out of stamina

### Animation
- Assets needed: Idle, Walk, Run, Sprint, Jump, HeavyAttack, Dodge, Hit, Death, Draw/Sheath

### Combat
CombatComponent:
- IsCombatEnabled
- Dodge, Block
- ���� �� ��� ���� ����

### Else
UI:
- Bind: OnPossess���� �ϱ�
- All UI: Z order (new depth = separate draw calls?)
- Focus:
> Page ���� Background Ŭ�� �� Page ����Ű �� ����

Data:
- Ŭ�������� ����� �ִ� BPClass ã�⸦ �ϳ��� DataAsset?�� ������?
- Class�� EditAnywhere ������ DataAsset?���� ó��

PlayerCharacter:
- Actions, bindings in generic way -> Map? Array?

ContextMenu:
- Lock ��ư, LockedImage
- Use/Equip, Lock/Unlock �� context�� ���� label
- PlayerMenu�� Inventory ���� �ٲ�� �ݱ�

Inventory:
- AddItem: Index �����ư�, �̹� ������ �� ĭ�� �ֱ� bFlag?
- DropItem: �� �ʸӿ� ������

Equipment:
- Equip �ȿ��� Unequip�� �� ���� slot�� �������� (�������)
> DesignatedIndex?

Design:
- ContextMenu HoveredImage ���ļ� Hover �ٲ�� ������ �ָ���
- TabButton Style: image + style? UImage + color?
- Hit damage

ESC:
- Name: GameMenu? PauseMenu? SystemMenu?
- Settings, Exit to MainMenu, Exit to Desktop

Animation:
- Idlebreak

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
- Interactable: Glowing effect
- ����Ʈ + �� ���� -> �̵�
- ��ħ��
- �߷� �̻�: ĳ���͸��� �߷��� �ٸ���? (https://dev.epicgames.com/community/learning/tutorials/w6l7/unreal-engine-custom-gravity-in-ue-5-4)

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
