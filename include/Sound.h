#ifndef SOUND_H
#define SOUND_H

#include <Arduino.h>
#include <M5Unified.h>
#include "Config.h"

class Sound {
private:
    int volume; // 0 a 10
    bool soundEnabled;
    bool bgmEnabled;
    float bgmTimer;
    int bgmStep;

public:
    Sound();

    void begin();
    void update(float dt);
    
    void playSound(SoundType type);
    void playTone(int frequency, int durationMs);

    void setVolume(int vol);
    int getVolume() const { return volume; }

    void setSoundEnabled(bool enabled) { soundEnabled = enabled; }
    bool isSoundEnabled() const { return soundEnabled; }

    void setBgmEnabled(bool enabled) { bgmEnabled = enabled; }
    bool isBgmEnabled() const { return bgmEnabled; }
};

#endif // SOUND_H
