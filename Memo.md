# Memo for docs for later

## UI
### UIManagementSubsystem: 전역 UI
PlayerMenu: 스탯, 장비, 인벤, 퀘스트, 맵, 업적, 설정, etc.

MainMenu: 로비, 세션, 크레딧, 옵션, etc.

LoadingScreen

### PlayerController: 플레이어 UI
ADemoHUD: DemoHUDWidget only?

DemoHUDWidget: Resource, Crosshair, Interact, QuickSlot, Minimap, etc.

구조: PlayerCharacter -> PlayerController -> HUD -> HUDWidget -> SubWidgets

이벤트: PlayerCharacter -> Broadcast -> PlayerController -> HUDWidget::Update(~) -> SubWidget::Update(~)


### 클래스 구조
BaseMenu: TabButtons, PageSwitcher, Close\
  PlayerMenu: StatsPage, EquipmentPage, InventoryPage, etc.\
  TraderMenu: BuyPage, SellPage\
StatsPage: ResourceBar, ResourceText\
EquipmentPage: ItemSlotWidget? EquipmentSlotWidget?\
InventoryPage: ItemTabSwitcher, ItemSlotWidget, Drag & Drop, ToolTip\
ContextMenu: Actions\
Button: UButton? CustomButton?
