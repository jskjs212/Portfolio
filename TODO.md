## Bugs

## TODO
### Inventory
- DropItem ����
- ValidateActionRequest: Index�� -1�̸� �տ������� ã�Ƽ� ó���ϱ⸦ �߰�����?

## check
### ��� Ŭ����: �ʱⰪ ������ �ʿ��� ���� ���� �˻��ϱ�

### ��ȣ�� �Լ� ���� ��Ȯ�� �ϱ�
Function(true) -> Function(ESomeOption::Good)
> 1���� ��������, ���ڰ� ������ �򰥸�

### ItemDataBase -> WeaponData, ArmorData
��ӵ� Data�� �������� DataTable�� �����Ǵµ� static_cast�� �ϳ��� ��Ȯ�� ���� ���� -> UObject? UDataAsset? (Weapon�ε� Armor casting�� != nullptr)
> WeaponData�� �ʿ��� ���� Weapon���� �� �����̹Ƿ�, GetRow<UWeaponData>�� �ٷ� ��������

## misc
### Life cycle
- ��� delegate.AddDynamic�� RemoveDynamic �ʿ� ����
- ��� �������� ���� �ֱ�?

### Inventory
- int32 AddItem() -> EAddItemResult? EItemValidationResult? EInventoryOpResult?

## Add
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