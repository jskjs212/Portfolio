## Bugs
�� ���̴� �繰 �ݱ� ����
- Visibility trace �߰�?

## TODO
### UI
PlayerHUDWidget�� Interact �ְ� ĵ���� ���̱�?

UUIManagementSubsystem: ���� UI
- PlayerMenu: ����, ���, �κ�, ����Ʈ, ��, ����, ����, etc.
- MainMenu: �κ�, ����, ũ����, �ɼ�, etc.
- LoadingScreen

PlayerController: �÷��̾� UI
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
### ��� Ŭ����: �ʱⰪ ������ �ʿ��� ���� ���� �˻��ϱ�

### Inventory
ValidateActionRequest: Index�� -1�̸� �տ������� ã�Ƽ� ó���ϱ⸦ �߰�����?

### PickUp
Box�� �߽��� Mesh�� Root�� ���� ��ġ�� �� ����.

## misc
### Life cycle
- ��� delegate.AddDynamic�� RemoveDynamic �ʿ� ����
- ��� �������� ���� �ֱ�?

### Inventory
- int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

### DataTable
- ���� StaticMesh�� Collision��, SkeletalMesh�� PhysicsAsset�� �������� �Ҵ������ ��.
> Valid üũ�� ����, ������ �ڵ����� ����������?

## Add
### Item::Tokens
Money, EXP, souls?

### �ɼ� ���
bIsWalkInputTogglesWalk, bIsSprintInputTogglesSprint

## Solved?

## Final tasks
### ����ȭ
AnimInstance: MultiThreadUpdate
Others: Multi-threading, replication not considered.

### ����
TODO:, check:, misc:, TEST: �� �ּ� Ȯ�� �� �����ϱ�\
�׽�Ʈ �ڵ� �����ϱ�

Log -> �߿��� ���� �˸� �޽��� �� ���� �������� ó���ϰ�, �������� LogCategory �����Ͽ� �߿��� �α׸� �����

## �ð��� �ȴٸ� ������ �κ�
### GAS

### CommonUI

### Optimization
