Memo for docs for later
=======================
## Common
Init vs Setup
- Init: One-time, internal setup only
- Setup: Might be repeated and involve external dependencies

Audio vs Sound
- Audio: System-level, global management (e.g., AudioSubsystem)
- Sound: Object-level, sound assets (e.g., SoundCues, SoundWaves)

## Character
BaseCharacter <- PlayerCharacter, AICharacter\
AICharacter <- Enemy, NPC�� �������, �ϳ����� ��� ó������ ���\
Enemy <- Boss, Mob?

### States
- Transition rules are set in:
> UStateManagerComponent::SetupAllowedTransitionsOnlyOnce()

## AI
### StateTree
- Inherit FStateTreeTaskCommonBase
> Can modify the parameters of the state tree by accessing the pointer from the context, by defining FStateTreePropertyRef in the instance data struct.\
> UCLASS���� USTRUCT�� �ٲٸ� �����Ϳ��� �ٷ� �ν��ϴµ�, USTRUCT���� UCLASS�� �ٲٸ� "Corrupt data found"�� ������ ��ü�� ���� �Ұ� -> �ش� Ŭ������ �ִ� uasset�� �����ؾ� �� -> Ŭ���� �ٲٱ� ���� �ڵ� ����ϰų�, Task �� �ش� Ŭ������ ���¿��� �����ϰ� �ٲٱ�
- Inherit UStateTreeTaskBlueprintBase
> Can define UFUNCTION that binds to the dynamic delegate in other systems.

## UI
### Manager
UIManagementSubsystem (���� UI)
- MainMenu
- LoadingScreen

PlayerController (�÷��̾� UI)
- DemoHUDWidget: ADemoHUD�� ����
- PlayerMenu: ���� ����

### Flow
PlayerCharacter -> Broadcast -> PlayerController -> HUDWidget-Update() -> SubWidget-Update()

### Ŭ���� ����
Term:
- Menu: The whole panel (container) that owns a set of tabs.
- Tab: The button that switches between pages.
- TabEntry: Tab's metadata linking to a page.
- Page: The content shown when a tab is active.
- Category: A second-level grouping of tabs inside a page like inventory (weapons, armors), and audio (music, SFX).

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
### Basic
Mouse: Look\
WASD: Move\
Shift: Sprint\
Ctrl: Walk\
F: Jump\
Space: Dodge\
Tab: PlayerMenu\
E: Interact\
Q: Toggle Lock-On

### UI
Global - Esc: Close\
PlayerMenu - Tab: Close\
PlayerMenu - Q, E: Switch tabs\
Inventory - 1~3: ItemTabs