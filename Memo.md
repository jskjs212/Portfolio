# Memo for docs for later
## Common
- Init: One-time, internal setup only
- Setup: Might be repeated and involve external dependencies

## Character
BaseCharacter <- PlayerCharacter, AICharacter\
AICharacter <- Enemy, NPC로 상속할지, 하나에서 모두 처리할지 고민\
Enemy <- Boss, Mob?

### States
- Transition rules are set in:
> UStateManagerComponent::SetupAllowedTransitionsOnlyOnce()

## UI
### Manager
UIManagementSubsystem (전역 UI)
- MainMenu
- LoadingScreen

PlayerController (플레이어 UI)
- DemoHUDWidget: ADemoHUD에 보관
- PlayerMenu: 직접 보관

### Flow
PlayerCharacter -> Broadcast -> PlayerController -> HUDWidget-Update() -> SubWidget-Update()

### 클래스 구조
Term:
- Menu: The whole panel (container) that owns a set of tabs.
- Tab: The button that switches between pages.
- TabEntry: Tab's metadata linking to a page.
- Page: The content shown when a tab is active.
- Category: A second-level grouping of tabs inside a page like inventory.

TabMenu: TabEntries (Tag, TabButton, Page), PageSwitcher
- PlayerMenu: StatsPage, EquipmentPage, InventoryPage (+ Skills, Quests, Map, Options, Achievements, Overall, etc.)
- TraderMenu: BuyPage, SellPage
- InventoryPage: CategoryEntries, WrapBoxes of categories which contain ItemSlotWidgets (like Weapon and Armor)

StatsPage: StatBar, StatText\
EquipmentPage: ItemSlotWidget? EquipmentSlotWidget?\
InventoryPage: TabMenu - WrapBoxes - ItemSlotWidget, Drag & Drop, ToolTip

ContextMenu: Actions\
DemoButton: UButton? CustomButton?

DemoHUDWidget: Resource, Crosshair, Interact, QuickSlot, Minimap, etc.

### Misc
Color: #F5F5E7

## Key bindings
Mouse: Look\
WASD: Move\
Shift: Sprint\
Ctrl: Walk\
Tab: PlayerMenu\
E: Interact\
Q: Toggle Lock-On

### UI
Global - Esc: Close\
PlayerMenu - Tab: Close\
PlayerMenu - Q, E: Switch tabs\
Inventory - 1~3: ItemTabs