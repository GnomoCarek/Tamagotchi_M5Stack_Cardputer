#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"
#include "Pet.h"
#include "Items.h"
#include "Achievements.h"
#include "Sound.h"

class StorageManager {
private:
    Preferences prefs;
    bool initialized;

public:
    StorageManager();

    bool begin();
    
    // Salvar & Carregar Estado Completo
    bool saveGame(const Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, float timeSpeed, int brightness);
    bool loadGame(Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, float& timeSpeed, int& brightness, uint32_t& outOfflineSeconds);

    bool resetSave();
    bool hasSaveData();
};

#endif // STORAGE_H
