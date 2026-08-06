#include "Clock.h"

Clock::Clock() {
    hours = 12;
    minutes = 0;
    seconds = 0;
    timeAccumulator = 0;
    timeSpeed = 1.0f;
}

void Clock::update(float dt) {
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
    if (hours >= 6 && hours < 8) return PHASE_DAWN;
    if (hours >= 8 && hours < 18) return PHASE_DAY;
    if (hours >= 18 && hours < 20) return PHASE_DUSK;
    return PHASE_NIGHT;
}

uint16_t Clock::getSkyColor() const {
    DayPhase phase = getDayPhase();
    switch (phase) {
        case PHASE_DAWN:  return COLOR_BG_DAWN;
        case PHASE_DAY:   return COLOR_BG_DAY;
        case PHASE_DUSK:  return COLOR_BG_DUSK;
        case PHASE_NIGHT: return COLOR_BG_NIGHT;
        default:          return COLOR_BG_DAY;
    }
}

void Clock::getFormattedTime(char* buffer, size_t size) const {
    snprintf(buffer, size, "%02d:%02d", hours, minutes);
}
