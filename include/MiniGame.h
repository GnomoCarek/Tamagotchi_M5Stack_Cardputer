#ifndef MINIGAME_H
#define MINIGAME_H

#include <Arduino.h>
#include <M5GFX.h>
#include "Config.h"
#include "Input.h"
#include "Animation.h"
#include "Sound.h"

enum MiniGameType {
    MINIGAME_CATCH_STARS = 0,
    MINIGAME_REFLEX_TEST
};

struct FallingItem {
    float x;
    float y;
    float speed;
    bool isStar; // true = estrela, false = bomba
    bool active;
};

#define MAX_FALLING_ITEMS 8

class MiniGame {
private:
    MiniGameType currentGameType;

    // Estado Geral
    int score;
    int coinsEarned;
    int lives;
    bool gameOver;

    // Estado - Capturar Estrelas
    float playerX;
    FallingItem items[MAX_FALLING_ITEMS];
    float spawnTimer;

    // Estado - Teste de Reflexo
    char targetChar;
    KeyAction targetAction;
    const char* targetName;
    float timeLimit;
    float timeRemaining;
    int comboStreak;
    int maxCombo;
    float reflexFeedbackTimer;
    bool reflexSuccess;

public:
    MiniGame();

    void reset(MiniGameType type = MINIGAME_CATCH_STARS);
    void update(float dt, KeyAction action, char typedChar, Sound& sound, Animation& anim);
    void draw(M5Canvas& canvas, Animation& anim);

    void spawnReflexTarget();

    bool isGameOver() const { return gameOver; }
    int getScore() const { return score; }
    int getCoinsEarned() const { return coinsEarned; }
    MiniGameType getType() const { return currentGameType; }
};

#endif // MINIGAME_H
