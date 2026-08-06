#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <M5GFX.h>
#include "Config.h"
#include "Input.h"
#include "Pet.h"
#include "Items.h"
#include "Achievements.h"
#include "Sound.h"
#include "Animation.h"
#include "Clock.h"

enum SubMenuType {
    SUBMENU_NONE = 0,
    SUBMENU_STATUS,
    SUBMENU_SHOP,
    SUBMENU_INVENTORY,
    SUBMENU_ACHIEVEMENTS,
    SUBMENU_SETTINGS
};

class MenuManager {
private:
    int mainIndex;
    int subIndex;
    SubMenuType activeSubMenu;
    int settingsOption;
    bool confirmReset;

public:
    MenuManager();

    void update(KeyAction action, Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, float& timeSpeed, int& brightness, bool& requestSave, bool& requestReset, GameState& currentState);
    void draw(M5Canvas& canvas, const Pet& pet, const ItemsManager& items, const AchievementsManager& achievements, const Sound& sound, const Clock& clock, Animation& anim, float timeSpeed, int brightness);

    int getMainIndex() const { return mainIndex; }
    SubMenuType getActiveSubMenu() const { return activeSubMenu; }
    void closeSubMenu() { activeSubMenu = SUBMENU_NONE; }
};

#endif // MENU_H
