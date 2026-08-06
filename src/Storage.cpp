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

bool StorageManager::saveGame(const Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, float timeSpeed, int brightness) {
    if (!initialized) begin();

    prefs.clear(); // Limpa estado antigo para evitar corrupção

    // Pet
    prefs.putString("pet_name", pet.name);
    prefs.putInt("pet_stage", (int)pet.stage);
    prefs.putInt("pet_var", (int)pet.variant);
    prefs.putInt("pet_pers", (int)pet.personality);

    prefs.putInt("pet_hunger", pet.hunger);
    prefs.putInt("pet_happy", pet.happiness);
    prefs.putInt("pet_energy", pet.energy);
    prefs.putInt("pet_health", pet.health);
    prefs.putInt("pet_weight", pet.weight);
    prefs.putInt("pet_age", pet.ageDays);
    prefs.putInt("pet_coins", pet.coins);

    prefs.putBool("pet_sleep", pet.isSleeping);
    prefs.putBool("pet_sick", pet.isSick);
    prefs.putBool("pet_dead", pet.isDead);
    prefs.putInt("pet_poop", pet.poopCount);

    prefs.putUInt("pet_time", pet.totalTimeSeconds);
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

    // Configurações
    prefs.putInt("snd_vol", sound.getVolume());
    prefs.putBool("snd_on", sound.isSoundEnabled());
    prefs.putBool("bgm_on", sound.isBgmEnabled());
    prefs.putFloat("time_spd", timeSpeed);
    prefs.putInt("scr_bright", brightness);

    // Timestamp
    uint32_t currentSecs = (uint32_t)(millis() / 1000);
    prefs.putUInt("last_ts", currentSecs);

    return true;
}

bool StorageManager::loadGame(Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, float& timeSpeed, int& brightness, uint32_t& outOfflineSeconds) {
    if (!hasSaveData()) return false;

    // Pet
    String pName = prefs.getString("pet_name", "Tama");
    strncpy(pet.name, pName.c_str(), sizeof(pet.name) - 1);
    pet.stage = (EvolutionStage)prefs.getInt("pet_stage", (int)STAGE_EGG);
    pet.variant = (PetVariant)prefs.getInt("pet_var", (int)PET_VARIANT_CLASSIC);
    pet.personality = (PersonalityType)prefs.getInt("pet_pers", (int)PERSONALITY_ALEGRE);

    pet.hunger = prefs.getInt("pet_hunger", DEFAULT_HUNGER);
    pet.happiness = prefs.getInt("pet_happy", DEFAULT_HAPPY);
    pet.energy = prefs.getInt("pet_energy", DEFAULT_ENERGY);
    pet.health = prefs.getInt("pet_health", DEFAULT_HEALTH);
    pet.weight = prefs.getInt("pet_weight", DEFAULT_WEIGHT);
    pet.ageDays = prefs.getInt("pet_age", 0);
    pet.coins = prefs.getInt("pet_coins", 50);

    pet.isSleeping = prefs.getBool("pet_sleep", false);
    pet.isSick = prefs.getBool("pet_sick", false);
    pet.isDead = prefs.getBool("pet_dead", false);
    pet.poopCount = prefs.getInt("pet_poop", 0);

    pet.totalTimeSeconds = prefs.getUInt("pet_time", 0);
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

    // Configurações
    sound.setVolume(prefs.getInt("snd_vol", 5));
    sound.setSoundEnabled(prefs.getBool("snd_on", true));
    sound.setBgmEnabled(prefs.getBool("bgm_on", false));
    timeSpeed = prefs.getFloat("time_spd", 1.0f);
    brightness = prefs.getInt("scr_bright", 128);

    // Tempo Offline
    uint32_t lastTS = prefs.getUInt("last_ts", 0);
    uint32_t currentSecs = (uint32_t)(millis() / 1000);
    if (currentSecs > lastTS && lastTS > 0) {
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
