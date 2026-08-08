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
    SUBMENU_SETTINGS,
    SUBMENU_WIFI
};

class MenuManager {
private:
    int mainIndex;
    int subIndex;
    SubMenuType activeSubMenu;
    ItemType activeCategory;
    int settingsOption;
    bool confirmReset;

    // Wi-Fi Config
    int wifiFieldStep; // 0 = SSID, 1 = Senha, 2 = Conectar
    char wifiSsidBuf[32];
    char wifiPassBuf[64];
    int wifiSsidLen;
    int wifiPassLen;
    char wifiStatusMsg[48];

    // Menu Inicial (Com Save)
    int initialMenuIndex; // 0 = Continuar, 1 = Novo Jogo, 2 = Config

    // Seleção de Sexo
    PetGender selectedGender;

    // Seleção de Minijogo
    int minigameSelectIndex; // 0 = Estrelas, 1 = Reflexo

public:
    MenuManager();

    void update(KeyAction action, char typedChar, Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, Clock& clock, int& brightness, bool& requestSave, bool& requestReset, GameState& currentState);
    void draw(M5Canvas& canvas, const Pet& pet, const ItemsManager& items, const AchievementsManager& achievements, const Sound& sound, const Clock& clock, Animation& anim, int brightness, GameState currentState);

    int getMainIndex() const { return mainIndex; }
    SubMenuType getActiveSubMenu() const { return activeSubMenu; }
    void openSubMenu(SubMenuType sub, ItemType cat = ITEM_FOOD);
    void closeSubMenu() { activeSubMenu = SUBMENU_NONE; }

    PetGender getSelectedGender() const { return selectedGender; }
    int getMinigameSelection() const { return minigameSelectIndex; }
    int getInitialMenuIndex() const { return initialMenuIndex; }
};

#endif // MENU_H
