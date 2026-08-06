#ifndef EVENTS_H
#define EVENTS_H

#include <Arduino.h>
#include <M5GFX.h>
#include "Config.h"
#include "Pet.h"
#include "Sound.h"

class EventsManager {
private:
    char currentNotification[48];
    float notificationTimer;
    float eventCheckTimer;

public:
    EventsManager();

    void update(float dt, Pet& pet, Sound& sound);
    void drawNotification(M5Canvas& canvas);
    
    void showMessage(const char* msg, float durationSec = 3.0f);
    bool hasActiveNotification() const { return notificationTimer > 0; }
};

#endif // EVENTS_H
