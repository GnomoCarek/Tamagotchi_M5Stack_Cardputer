#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>
#include <M5GFX.h>
#include "Config.h"

// Particle types
enum ParticleType {
    PARTICLE_HEART = 0,
    PARTICLE_STAR,
    PARTICLE_NOTE,
    PARTICLE_SPARKLE
};

struct Particle {
    float x;
    float y;
    float vx;
    float vy;
    float life; // 1.0 to 0.0
    ParticleType type;
    uint16_t color;
    bool active;
};

#define MAX_PARTICLES 16

class Animation {
private:
    Particle particles[MAX_PARTICLES];
    float animTimer;
    int currentFrame;

public:
    Animation();

    void update(float dt);
    void spawnParticle(float x, float y, ParticleType type, uint16_t color = COLOR_HAPPY);
    void drawParticles(M5Canvas& canvas);

    // Dynamic Sprite Rendering
    void drawPetSprite(M5Canvas& canvas, int centerX, int centerY, EvolutionStage stage, PetVariant variant, const char* stateText, bool isSleeping, bool isSick, bool isDead, int happyLevel);
    
    // Pixel Art Icons
    void drawIcon(M5Canvas& canvas, int x, int y, int iconIndex, uint16_t color);
    void drawPoop(M5Canvas& canvas, int x, int y);
    void drawStar(M5Canvas& canvas, int x, int y, uint16_t color);
    void drawBomb(M5Canvas& canvas, int x, int y);
    void drawFood(M5Canvas& canvas, int x, int y, int foodType);
};

#endif // ANIMATION_H
