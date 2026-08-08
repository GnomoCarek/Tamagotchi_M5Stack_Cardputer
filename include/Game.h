#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <M5Cardputer.h>
#include "Config.h"
#include "Pet.h"
#include "Clock.h"
#include "Animation.h"
#include "Input.h"
#include "Sound.h"
#include "Storage.h"
#include "Items.h"
#include "Achievements.h"
#include "MiniGame.h"
#include "Events.h"
#include "Menu.h"
#include "Renderer.h"

class Game {
private:
    Pet pet;
    Clock clock;
    Animation animation;
    InputManager input;
    Sound sound;
    StorageManager storage;
    ItemsManager items;
    AchievementsManager achievements;
    MiniGame minigame;
    EventsManager events;
    MenuManager menu;
    Renderer renderer;

    GameState currentState;
    EvolutionStage lastStage;
    int brightness;
    uint32_t lastFrameTime;
    float autoSaveTimer;
    float hatchingTimer;

    char nameInput[16];
    int nameLen;

    float screenIdleTimer;
    bool isScreenOff;

public:
    Game();

    void begin();
    void update();

    void saveGame();
    void resetGame();
};

#endif // GAME_H
