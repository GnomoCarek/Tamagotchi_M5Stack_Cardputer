#include "Events.h"

EventsManager::EventsManager() {
    currentNotification[0] = '\0';
    notificationTimer = 0;
    eventCheckTimer = 0;
}

void EventsManager::showMessage(const char* msg, float durationSec) {
    strncpy(currentNotification, msg, sizeof(currentNotification) - 1);
    currentNotification[sizeof(currentNotification) - 1] = '\0';
    notificationTimer = durationSec;
}

void EventsManager::update(float dt, Pet& pet, Sound& sound) {
    if (notificationTimer > 0) {
        notificationTimer -= dt;
        if (notificationTimer <= 0) {
            currentNotification[0] = '\0';
        }
    }

    if (pet.isDead) return;

    // Checagem de eventos aleatórios a cada ~40s
    eventCheckTimer += dt;
    if (eventCheckTimer >= 40.0f) {
        eventCheckTimer = 0;

        int randEvent = random(0, 100);
        if (pet.hunger < 30) {
            showMessage("Estou com fome!", 4.0f);
            sound.playSound(SOUND_CLICK);
        } else if (pet.happiness < 30) {
            showMessage("Vamos brincar?", 4.0f);
            sound.playSound(SOUND_CLICK);
        } else if (pet.energy < 20 && !pet.isSleeping) {
            showMessage("Estou cansado...", 4.0f);
            sound.playSound(SOUND_SLEEP);
        } else if (randEvent < 25) { // Encontrou moedas
            int foundCoins = random(3, 12);
            pet.addCoins(foundCoins);
            char buf[48];
            snprintf(buf, sizeof(buf), "Encontrei +%d Moedas!", foundCoins);
            showMessage(buf, 3.5f);
            sound.playSound(SOUND_COIN);
        } else if (randEvent < 45 && pet.happiness > 70) {
            showMessage("Estou muito feliz!", 3.0f);
            sound.playSound(SOUND_PLAY);
        }
    }
}

void EventsManager::drawNotification(M5Canvas& canvas) {
    if (notificationTimer <= 0 || currentNotification[0] == '\0') return;

    int boxW = 200;
    int boxH = 22;
    int boxX = (SCREEN_WIDTH - boxW) / 2;
    int boxY = 22;

    canvas.fillRoundRect(boxX, boxY, boxW, boxH, 4, COLOR_PANEL_BG);
    canvas.drawRoundRect(boxX, boxY, boxW, boxH, 4, COLOR_COIN);

    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString(currentNotification, SCREEN_WIDTH / 2, boxY + (boxH / 2));
}
