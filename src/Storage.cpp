#include "Storage.h"
#include <time.h>

#define PREF_NAMESPACE "tamagotchi"

StorageManager::StorageManager() {
    initialized = false;
}

bool StorageManager::begin() {
    initialized = prefs.begin(PREF_NAMESPACE, false);
    return initialized;
}

bool StorageManager::hasSaveData() {
    if (!initialized) begin();
    return prefs.isKey("pet_name");
}

bool StorageManager::saveGame(const Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, Clock& clock, int brightness) {
    if (!initialized) begin();

    prefs.clear(); // Limpa estado antigo para evitar corrupção

    // Pet
    prefs.putString("pet_name", pet.name);
    prefs.putInt("pet_gender", (int)pet.gender);
    prefs.putInt("pet_stage", (int)pet.stage);
    prefs.putInt("pet_var", (int)pet.variant);
    prefs.putInt("pet_pers", (int)pet.personality);

    prefs.putInt("pet_hunger", pet.hunger);
    prefs.putInt("pet_happy", pet.happiness);
    prefs.putInt("pet_energy", pet.energy);
    prefs.putInt("pet_health", pet.health);
    prefs.putInt("pet_weight", pet.weight);
    prefs.putInt("pet_affect", pet.affection);
    prefs.putInt("pet_age", pet.ageDays);
    prefs.putInt("pet_coins", pet.coins);

    prefs.putBool("pet_sleep", pet.isSleeping);
    prefs.putBool("pet_sick", pet.isSick);
    prefs.putBool("pet_dead", pet.isDead);
    prefs.putBool("pet_dirty", pet.isDirty);
    prefs.putInt("pet_poop", pet.poopCount);

    prefs.putUInt("pet_time", pet.totalTimeSeconds);
    prefs.putUInt("pet_last_bath", pet.lastBathTimeSeconds);
    prefs.putFloat("pet_sick_dur", pet.sickDurationSeconds);
    prefs.putInt("pet_meals", pet.mealsEaten);
    prefs.putInt("pet_baths", pet.bathsTaken);
    prefs.putInt("pet_games", pet.gamesPlayed);
    prefs.putInt("pet_meds", pet.medicinesGiven);

    // Inventário
    int inv[ITEM_CATALOG_SIZE];
    items.saveInventory(inv);
    prefs.putBytes("items_inv", inv, sizeof(inv));

    // Conquistas
    bool ach[MAX_ACHIEVEMENTS];
    achievements.saveState(ach);
    prefs.putBytes("ach_state", ach, sizeof(ach));

    // Configurações & Wi-Fi & Dev Mode
    prefs.putInt("snd_vol", sound.getVolume());
    prefs.putBool("snd_on", sound.isSoundEnabled());
    prefs.putBool("bgm_on", sound.isBgmEnabled());
    prefs.putBool("dev_mode", clock.getDevMode());
    prefs.putString("wifi_ssid", clock.getWifiSsid());
    prefs.putString("wifi_pass", clock.getWifiPass());
    prefs.putInt("scr_bright", brightness);
    prefs.putInt("scr_timeout", pet.screenTimeoutSec);

    // Timestamp (epoch time via time(nullptr))
    uint32_t currentSecs = (uint32_t)time(nullptr);
    prefs.putUInt("last_ts", currentSecs);

    return true;
}

bool StorageManager::loadGame(Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, Clock& clock, int& brightness, uint32_t& outOfflineSeconds) {
    if (!hasSaveData()) return false;

    // Pet
    String pName = prefs.getString("pet_name", "Tama");
    strncpy(pet.name, pName.c_str(), sizeof(pet.name) - 1);
    pet.gender = (PetGender)prefs.getInt("pet_gender", (int)GENDER_MALE);
    pet.stage = (EvolutionStage)prefs.getInt("pet_stage", (int)STAGE_EGG);
    pet.variant = (PetVariant)prefs.getInt("pet_var", (int)PET_VARIANT_CLASSIC);
    pet.personality = (PersonalityType)prefs.getInt("pet_pers", (int)PERSONALITY_ALEGRE);

    pet.hunger = prefs.getInt("pet_hunger", DEFAULT_HUNGER);
    pet.happiness = prefs.getInt("pet_happy", DEFAULT_HAPPY);
    pet.energy = prefs.getInt("pet_energy", DEFAULT_ENERGY);
    pet.health = prefs.getInt("pet_health", DEFAULT_HEALTH);
    pet.weight = prefs.getInt("pet_weight", DEFAULT_WEIGHT);
    pet.affection = prefs.getInt("pet_affect", 80);
    pet.ageDays = prefs.getInt("pet_age", 0);
    pet.coins = prefs.getInt("pet_coins", 50);

    pet.isSleeping = prefs.getBool("pet_sleep", false);
    pet.isSick = prefs.getBool("pet_sick", false);
    pet.isDead = prefs.getBool("pet_dead", false);
    pet.isDirty = prefs.getBool("pet_dirty", false);
    pet.poopCount = prefs.getInt("pet_poop", 0);

    pet.totalTimeSeconds = prefs.getUInt("pet_time", 0);
    pet.lastBathTimeSeconds = prefs.getUInt("pet_last_bath", 0);
    pet.sickDurationSeconds = prefs.getFloat("pet_sick_dur", 0.0f);
    pet.mealsEaten = prefs.getInt("pet_meals", 0);
    pet.bathsTaken = prefs.getInt("pet_baths", 0);
    pet.gamesPlayed = prefs.getInt("pet_games", 0);
    pet.medicinesGiven = prefs.getInt("pet_meds", 0);

    // Inventário
    int inv[ITEM_CATALOG_SIZE];
    if (prefs.getBytes("items_inv", inv, sizeof(inv)) == sizeof(inv)) {
        items.loadInventory(inv);
    }

    // Conquistas
    bool ach[MAX_ACHIEVEMENTS];
    if (prefs.getBytes("ach_state", ach, sizeof(ach)) == sizeof(ach)) {
        achievements.loadState(ach);
    }

    // Configurações, Relógio & Wi-Fi
    sound.setVolume(prefs.getInt("snd_vol", 5));
    sound.setSoundEnabled(prefs.getBool("snd_on", true));
    sound.setBgmEnabled(prefs.getBool("bgm_on", false));
    
    bool devMode = prefs.getBool("dev_mode", false);
    clock.setDevMode(devMode);

    String ssid = prefs.getString("wifi_ssid", "");
    String pass = prefs.getString("wifi_pass", "");
    clock.setWifiCredentials(ssid.c_str(), pass.c_str());
    if (ssid.length() > 0) {
        clock.begin(); // Tenta sincronizar Wi-Fi e NTP
    }

    brightness = prefs.getInt("scr_bright", 128);
    pet.screenTimeoutSec = prefs.getInt("scr_timeout", 60);

    // Tempo Offline
    uint32_t lastTS = prefs.getUInt("last_ts", 0);
    uint32_t currentSecs = (uint32_t)time(nullptr);
    if (currentSecs > lastTS && lastTS > 100000) {
        outOfflineSeconds = currentSecs - lastTS;
    } else {
        outOfflineSeconds = 0;
    }

    return true;
}

bool StorageManager::resetSave() {
    if (!initialized) begin();
    return prefs.clear();
}
