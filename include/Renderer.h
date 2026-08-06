#ifndef RENDERER_H
#define RENDERER_H

#include <Arduino.h>
#include <M5GFX.h>
#include "Config.h"
#include "Pet.h"
#include "Clock.h"
#include "Animation.h"
#include "Menu.h"
#include "Events.h"
#include "Items.h"
#include "Achievements.h"
#include "Sound.h"

class Renderer {
private:
    M5Canvas canvas;
    bool initialized;

public:
    Renderer();

    void begin();
    
    void renderGameplay(const Pet& pet, const Clock& clock, Animation& anim, EventsManager& events, MenuManager& menu, const ItemsManager& items, const AchievementsManager& achievements, const Sound& sound, GameState currentState, float timeSpeed, int brightness);
    
    void renderTitleScreen(Animation& anim);
    void renderEvolutionScreen(const Pet& pet, Animation& anim);
    void renderGameOverScreen(const Pet& pet, Animation& anim);

    M5Canvas& getCanvas() { return canvas; }
};

#endif // RENDERER_H
