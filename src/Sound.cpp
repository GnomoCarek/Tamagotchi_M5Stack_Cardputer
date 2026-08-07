#include "Sound.h"

Sound::Sound() {
    volume = 5;
    soundEnabled = true;
    bgmEnabled = false;
    bgmTimer = 0;
    bgmStep = 0;
}

void Sound::begin() {
    M5.Speaker.setVolume(volume * 25); // Converte 0-10 para 0-250
}

void Sound::setVolume(int vol) {
    volume = constrain(vol, 0, 10);
    M5.Speaker.setVolume(volume * 25);
}

void Sound::playTone(int frequency, int durationMs) {
    if (!soundEnabled || volume == 0) return;
    M5.Speaker.tone(frequency, durationMs);
}

void Sound::playSound(SoundType type) {
    if (!soundEnabled || volume == 0) return;

    switch (type) {
        case SOUND_CLICK:
            M5.Speaker.tone(1200, 30);
            break;
            
        case SOUND_FEED: // ♪ bip bip
            M5.Speaker.tone(900, 60);
            delay(80);
            M5.Speaker.tone(1400, 80);
            break;
            
        case SOUND_PLAY: // ♪ pipipi
            M5.Speaker.tone(1500, 50);
            delay(60);
            M5.Speaker.tone(1800, 50);
            delay(60);
            M5.Speaker.tone(2200, 70);
            break;
            
        case SOUND_SLEEP: // ♪ zzz zzz
            M5.Speaker.tone(400, 150);
            delay(180);
            M5.Speaker.tone(350, 150);
            break;
            
        case SOUND_SICK: // ♪ biiiiip
            M5.Speaker.tone(300, 400);
            break;
            
        case SOUND_POOP: // ♪ poin poin poin
            for (int i = 0; i < 3; i++) {
                M5.Speaker.tone(600 - (i * 100), 70);
                delay(80);
            }
            break;
            
        case SOUND_EVOLUTION: // ♪ pequena melodia
            M5.Speaker.tone(523, 100); // Do
            delay(120);
            M5.Speaker.tone(659, 100); // Mi
            delay(120);
            M5.Speaker.tone(784, 100); // Sol
            delay(120);
            M5.Speaker.tone(1046, 250); // Do agudo
            break;
            
        case SOUND_DEATH: // ♪ melodia triste
            M5.Speaker.tone(400, 200);
            delay(220);
            M5.Speaker.tone(350, 200);
            delay(220);
            M5.Speaker.tone(300, 400);
            break;
            
        case SOUND_BOOT: // ♪ som clássico Tamagotchi
            M5.Speaker.tone(1000, 60);
            delay(70);
            M5.Speaker.tone(1500, 60);
            delay(70);
            M5.Speaker.tone(2000, 120);
            break;

        case SOUND_COIN:
            M5.Speaker.tone(987, 60);
            delay(70);
            M5.Speaker.tone(1318, 150);
            break;

        case SOUND_ACHIEVEMENT:
            M5.Speaker.tone(659, 80);
            delay(90);
            M5.Speaker.tone(880, 80);
            delay(90);
            M5.Speaker.tone(1175, 200);
            break;

        case SOUND_CLEAN:
            M5.Speaker.tone(1200, 80);
            delay(90);
            M5.Speaker.tone(1600, 80);
            break;

        case SOUND_EGG_HATCH:
            M5.Speaker.tone(440, 80);
            delay(90);
            M5.Speaker.tone(554, 80);
            delay(90);
            M5.Speaker.tone(659, 80);
            delay(90);
            M5.Speaker.tone(880, 200);
            break;

        case SOUND_SCREEN_SLEEP:
            M5.Speaker.tone(600, 50);
            delay(60);
            M5.Speaker.tone(300, 80);
            break;

        case SOUND_SCREEN_WAKE:
            M5.Speaker.tone(400, 50);
            delay(60);
            M5.Speaker.tone(800, 80);
            break;

        case SOUND_LOW_BATTERY:
            M5.Speaker.tone(300, 150);
            delay(180);
            M5.Speaker.tone(200, 250);
            break;

        case SOUND_TYPING:
            M5.Speaker.tone(1400, 20);
            break;
    }
}

void Sound::update(float dt) {
    if (!bgmEnabled || !soundEnabled || volume == 0) return;

    bgmTimer += dt;
    if (bgmTimer >= 0.4f) {
        bgmTimer = 0;
        // Melodia de fundo simples (8 notas)
        static const int notes[] = {523, 587, 659, 698, 784, 698, 659, 587};
        M5.Speaker.tone(notes[bgmStep], 80);
        bgmStep = (bgmStep + 1) % 8;
    }
}
