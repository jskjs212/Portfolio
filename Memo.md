# Memo for docs for later

## UI
### UIManagementSubsystem: ���� UI
PlayerMenu: ����, ���, �κ�, ����Ʈ, ��, ����, ����, etc.

MainMenu: �κ�, ����, ũ����, �ɼ�, etc.

LoadingScreen

### PlayerController: �÷��̾� UI
ADemoHUD: DemoHUDWidget only?

DemoHUDWidget: Resource, Crosshair, Interact, QuickSlot, Minimap, etc.

����: PlayerCharacter -> PlayerController -> HUD -> HUDWidget -> SubWidgets

�̺�Ʈ: PlayerCharacter -> Broadcast -> PlayerController -> HUDWidget::Update(~) -> SubWidget::Update(~)


### Ŭ���� ����
BaseMenu: TabButtons, PageSwitcher, Close\
  PlayerMenu: StatsPage, EquipmentPage, InventoryPage, etc.\
  TraderMenu: BuyPage, SellPage\
StatsPage: ResourceBar, ResourceText\
EquipmentPage: ItemSlotWidget? EquipmentSlotWidget?\
InventoryPage: ItemTabSwitcher, ItemSlotWidget, Drag & Drop, ToolTip\
ContextMenu: Actions\
Button: UButton? CustomButton?
