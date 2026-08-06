#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include "Config.h"

enum DayPhase {
    PHASE_DAWN = 0,
    PHASE_DAY,
    PHASE_DUSK,
    PHASE_NIGHT
};

class Clock {
private:
    int hours;
    int minutes;
    int seconds;
    float timeAccumulator;
    float timeSpeed; // Multiplicador de velocidade (1.0 = real, >1.0 = acelera)

public:
    Clock();

    void update(float dt);
    void setTime(int h, int m, int s = 0);
    
    int getHours() const { return hours; }
    int getMinutes() const { return minutes; }
    int getSeconds() const { return seconds; }
    
    void setTimeSpeed(float speed) { timeSpeed = speed; }
    float getTimeSpeed() const { return timeSpeed; }

    DayPhase getDayPhase() const;
    uint16_t getSkyColor() const;
    void getFormattedTime(char* buffer, size_t size) const;
};

#endif // CLOCK_H
