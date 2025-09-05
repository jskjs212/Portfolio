# Memo for docs for later
## Character
BaseCharacter <- PlayerCharacter, AICharacter\
AICharacter <- Enemy, NPC로 상속할지, 하나에서 모두 처리할지 고민\
Enemy <- Boss, Mob?

## UI
### Manager
UIManagementSubsystem (전역 UI)
- MainMenu, LoadingScreen

PlayerController (플레이어 UI)
- ADemoHUD, PlayerMenu

### Flow
PlayerCharacter -> Broadcast -> PlayerController -> HUDWidget-Update() -> SubWidget-Update()

### 클래스 구조
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