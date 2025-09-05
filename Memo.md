# Memo for docs for later
## Character
BaseCharacter <- PlayerCharacter, AICharacter\
AICharacter <- Enemy, NPC�� �������, �ϳ����� ��� ó������ ���\
Enemy <- Boss, Mob?

## UI
### Manager
UIManagementSubsystem (���� UI)
- MainMenu, LoadingScreen

PlayerController (�÷��̾� UI)
- ADemoHUD, PlayerMenu

### Flow
PlayerCharacter -> Broadcast -> PlayerController -> HUDWidget-Update() -> SubWidget-Update()

### Ŭ���� ����
BaseMenu: TabButtons, PageSwitcher, Switch, Close
- PlayerMenu: StatsPage, EquipmentPage, InventoryPage, etc.
- TraderMenu: BuyPage, SellPage

StatsPage: ResourceBar, ResourceText\
EquipmentPage: ItemSlotWidget? EquipmentSlotWidget?\
InventoryPage: ItemTabSwitcher, ItemSlotWidget, Drag & Drop, ToolTip

ContextMenu: Actions\
Button: UButton? CustomButton?

DemoHUDWidget: Resource, Crosshair, Interact, QuickSlot, Minimap, etc.

## Key bindings
Mouse: Look\
WASD: Move\
Shift: Sprint\
Ctrl: Walk\
Tab: PlayerMenu\
E: Interact