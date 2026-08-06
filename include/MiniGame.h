#ifndef MINIGAME_H
#define MINIGAME_H

#include <Arduino.h>
#include <M5GFX.h>
#include "Config.h"
#include "Input.h"
#include "Animation.h"
#include "Sound.h"

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
    float playerX;
    int score;
    int coinsEarned;
    int lives;
    bool gameOver;
    FallingItem items[MAX_FALLING_ITEMS];
    float spawnTimer;

public:
    MiniGame();

    void reset();
    void update(float dt, KeyAction action, Sound& sound, Animation& anim);
    void draw(M5Canvas& canvas, Animation& anim);

    bool isGameOver() const { return gameOver; }
    int getScore() const { return score; }
    int getCoinsEarned() const { return coinsEarned; }
};

#endif // MINIGAME_H
