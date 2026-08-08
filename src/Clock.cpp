#include "Clock.h"

Clock::Clock() {
    hours = 12;
    minutes = 0;
    seconds = 0;
    timeAccumulator = 0;
    timeSpeed = 1.0f;
    isDevMode = false;

    wifiConnected = false;
    ntpSynced = false;
    wifiSsid[0] = '\0';
    wifiPass[0] = '\0';
    lastNtpAttempt = 0;
}

void Clock::begin() {
    if (wifiSsid[0] != '\0') {
        tryConnectWifi(3000);
    }
}

void Clock::setDevMode(bool enable) {
    isDevMode = enable;
    if (isDevMode) {
        timeSpeed = (24.0f * 3600.0f) / DEV_MODE_DAY_DURATION_SEC; // 48.0x
    } else {
        timeSpeed = 1.0f;
    }
}

void Clock::setWifiCredentials(const char* ssid, const char* pass) {
    if (ssid) {
        strncpy(wifiSsid, ssid, sizeof(wifiSsid) - 1);
        wifiSsid[sizeof(wifiSsid) - 1] = '\0';
    }
    if (pass) {
        strncpy(wifiPass, pass, sizeof(wifiPass) - 1);
        wifiPass[sizeof(wifiPass) - 1] = '\0';
    }
}

bool Clock::tryConnectWifi(uint32_t timeoutMs) {
    if (wifiSsid[0] == '\0') return false;

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSsid, wifiPass);

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start < timeoutMs)) {
        delay(50);
    }

    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        syncNTP();
        return true;
    } else {
        wifiConnected = false;
        return false;
    }
}

bool Clock::syncNTP() {
    if (WiFi.status() != WL_CONNECTED) return false;

    // Configura NTP para fuso horário de Brasília (UTC-3)
    configTime(-3 * 3600, 0, "a.st1.ntp.br", "b.st1.ntp.br", "pool.ntp.org");

    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 3000)) {
        hours = timeinfo.tm_hour;
        minutes = timeinfo.tm_min;
        seconds = timeinfo.tm_sec;
        ntpSynced = true;
        lastNtpAttempt = millis();
        return true;
    }

    return false;
}

void Clock::update(float dt) {
    // Em modo normal com NTP sincronizado e Wi-Fi conectado, consulta o relógio real a cada 10s
    if (!isDevMode && wifiConnected && ntpSynced) {
        static float rtcCheckTimer = 0;
        rtcCheckTimer += dt;
        if (rtcCheckTimer >= 10.0f) {
            rtcCheckTimer = 0;
            struct tm timeinfo;
            if (getLocalTime(&timeinfo, 10)) {
                hours = timeinfo.tm_hour;
                minutes = timeinfo.tm_min;
                seconds = timeinfo.tm_sec;
                return;
            }
        }
    }

    // Se offline ou em Dev Mode, avança o tempo via acumulador e multiplicador
    timeAccumulator += dt * timeSpeed;
    while (timeAccumulator >= 1.0f) {
        timeAccumulator -= 1.0f;
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours = (hours + 1) % 24;
            }
        }
    }
}

void Clock::setTime(int h, int m, int s) {
    hours = h % 24;
    minutes = m % 60;
    seconds = s % 60;
}

DayPhase Clock::getDayPhase() const {
    // 06:00–11:59 : Manhã
    // 12:00–17:59 : Tarde
    // 18:00–18:59 : Pôr do Sol
    // 19:00–05:59 : Noite
    if (hours >= 6 && hours < 12) return PHASE_MORNING;
    if (hours >= 12 && hours < 18) return PHASE_AFTERNOON;
    if (hours == 18) return PHASE_DUSK;
    return PHASE_NIGHT;
}

uint16_t Clock::getSkyColor() const {
    DayPhase phase = getDayPhase();
    switch (phase) {
        case PHASE_MORNING:   return COLOR_BG_DAY;    // Azul suave
        case PHASE_AFTERNOON: return 0x2CDD;          // Azul celeste vibrante
        case PHASE_DUSK:      return COLOR_BG_DAWN;   // Alaranjado / Pôr do sol
        case PHASE_NIGHT:     return COLOR_BG_NIGHT;  // Azul escuro
        default:              return COLOR_BG_DAY;
    }
}

void Clock::getFormattedTime(char* buffer, size_t size) const {
    snprintf(buffer, size, "%02d:%02d", hours, minutes);
}
