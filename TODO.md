## Bugs

## TODO
### PickUp
IPickUpable to AItem?
> Sphere -> overlap -> ĳ������ NearbyItems�� �߰� -> Tick���� ���� �´� ������ ǥ�� -> EŰ ������ �ݱ� -> NearbyItems���� ����?

### UI
Button: UButton? CustomButton?
MainMenu: Stats, Equipment, Inventory, etc.
Stats: ResourceBar, ResourceText
Equipment: ItemSlotWidget?
Inventory: ItemTab, ItemSlotWidget, Drag & Drop, ToolTip
ContextMenu: Actions

## check
### ��� Ŭ����: �ʱⰪ ������ �ʿ��� ���� ���� �˻��ϱ�

### ��ȣ�� �Լ� ���� ��Ȯ�� �ϱ�
Function(true) -> Function(ESomeOption::Good)
> 1���� ��������, ���ڰ� ������ �򰥸�

### Inventory
- ValidateActionRequest: Index�� -1�̸� �տ������� ã�Ƽ� ó���ϱ⸦ �߰�����?

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