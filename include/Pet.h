#ifndef PET_H
#define PET_H

#include <Arduino.h>
#include "Config.h"

class Pet {
public:
    char name[16];
    EvolutionStage stage;
    PetVariant variant;
    PersonalityType personality;

    // Atributos de 0 a 100
    int hunger;
    int happiness;
    int energy;
    int health;
    int weight;
    int ageDays;
    int coins;

    // Estados
    bool isSleeping;
    bool isSick;
    bool isDead;
    int poopCount;

    // Contadores & Estatísticas
    uint32_t totalTimeSeconds;
    int mealsEaten;
    int bathsTaken;
    int gamesPlayed;
    int medicinesGiven;
    int hoursSlept;
    int careMistakes;

    // Timers internos de decaimento
    float hungerTimer;
    float happinessTimer;
    float energyTimer;
    float healthTimer;
    float poopTimer;

    Pet();

    void reset(const char* petName, PetVariant petVar = PET_VARIANT_CLASSIC);
    void update(float dt);
    void applyOfflineTime(uint32_t elapsedSeconds);

    // Ações
    bool feed(int hungerGain, int weightGain = 1);
    bool play(int happyGain, int energyCost = 5);
    bool cure();
    bool cleanPoop();
    bool toggleSleep();
    bool petCare();
    void addCoins(int amount);

    // Evolução e Mudança de Estado
    void checkEvolution();
    void triggerPoop();
    void checkSickness();

    // Utilitários
    const char* getPersonalityName() const;
    const char* getStageName() const;
    const char* getStatusText() const;
};

#endif // PET_H
