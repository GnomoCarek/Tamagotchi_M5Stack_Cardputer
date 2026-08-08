#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "Config.h"

enum DayPhase {
    PHASE_MORNING = 0, // 06:00 - 11:59 (Manhã)
    PHASE_AFTERNOON,  // 12:00 - 17:59 (Tarde)
    PHASE_DUSK,       // 18:00 - 18:59 (Pôr do Sol)
    PHASE_NIGHT       // 19:00 - 05:59 (Noite)
};

class Clock {
private:
    int hours;
    int minutes;
    int seconds;
    float timeAccumulator;
    float timeSpeed; // Multiplicador de velocidade (1.0 = real, >1.0 = acelera)
    bool isDevMode;

    bool wifiConnected;
    bool ntpSynced;
    char wifiSsid[32];
    char wifiPass[64];
    uint32_t lastNtpAttempt;

public:
    Clock();

    void begin();
    void update(float dt);
    void setTime(int h, int m, int s = 0);
    
    int getHours() const { return hours; }
    int getMinutes() const { return minutes; }
    int getSeconds() const { return seconds; }
    
    void setTimeSpeed(float speed) { timeSpeed = speed; }
    float getTimeSpeed() const { return timeSpeed; }

    void setDevMode(bool enable);
    bool getDevMode() const { return isDevMode; }

    // Wi-Fi e NTP
    void setWifiCredentials(const char* ssid, const char* pass);
    bool tryConnectWifi(uint32_t timeoutMs = 5000);
    bool syncNTP();
    bool isWifiConnected() const { return wifiConnected; }
    bool isNtpSynced() const { return ntpSynced; }
    const char* getWifiSsid() const { return wifiSsid; }
    const char* getWifiPass() const { return wifiPass; }

    DayPhase getDayPhase() const;
    uint16_t getSkyColor() const;
    void getFormattedTime(char* buffer, size_t size) const;
};

#endif // CLOCK_H
