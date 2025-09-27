## Bugs
- UDemoHUDWidget::UpdateLockOnPosition: �ణ �����ʿ� ǥ�õ�

## TODO
### Animation
- Assets needed: Pivot, Idle, Walk, Run, Sprint, Jump, HeavyAttack, Dodge, Hit, Death, Draw/Sheath

### Combat
CombatComponent:
- IsCombatEnabled
- Dodge, Block
- ���� �� ��� ���� ����
- Ÿ�ݰ�

Attack:
- CollisionComponent?
- Multiple sockets in actor -> line traces from last frame?
- HitBox? HitFrame?

### Else
UI:
- AI health bar image
- Bind: OnPossess���� �ϱ�
- All UI: Z order (new depth = separate draw calls?)
- Focus: Page ���� Background Ŭ�� �� Page ����Ű �� ����
- Damage number
- HUD�� tabŰ �ȳ�
- DragDropOp: ItemSlotDragSource? What happens if drag from Inventory to Equipment?
- WBP_TabButton: Button style
- Cache RowHandle in ItemInfo

ItemActionDispatcher:
- Names: Include inventory, equipment
- Too many delegates?

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
- BlendSpace - �ڿ�������

Boss:
- Boss BT
- AIStatus in HUD
- Intro sequence?

## check
### ��� Ŭ����: �ʱⰪ ������ �ʿ��� ���� ���� �˻��ϱ�

### Inventory
ValidateActionRequest: Index�� -1�̸� �տ������� ã�Ƽ� ó���ϱ⸦ �߰�����?

### PickUp
Box�� �߽��� ������ ������ �� Mesh�� Root�� ���� ��ġ�� �� ����.

### Optimize
- UTargetingComponent::FindTarget()

## misc
### Life cycle
- ��� delegate.AddDynamic�� RemoveDynamic �ʿ� ����
- ��� �������� ���� �ֱ�?

### UX
- SFX: Drop
- LockOn ����� �� �ڷ� ���� ����
> �տ� ��� �������� ��ü�� �����Ǹ� ������

## Add
### Item::Tokens
Money, EXP, souls?

### �ɼ� ���
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

### Audio
MusicComponent? MusicSystem in GameInstance? SoundClasses?

### Misc
- Change LockOn target
- LockOn disadvantages: Can't sprint?
- Interactable: Glowing effect
- ����Ʈ + �� ���� -> �̵�
- ��ħ��
- �߷� �̻�: ĳ���͸��� �߷��� �ٸ���? (https://dev.epicgames.com/community/learning/tutorials/w6l7/unreal-engine-custom-gravity-in-ue-5-4)
- ��� ���� �ݱ�
- Targeting point�� Ư�� Socket���� ����
- �����Կ��� ���� ����

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
