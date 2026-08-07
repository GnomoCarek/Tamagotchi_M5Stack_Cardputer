#include "Pet.h"

Pet::Pet() {
    reset("Tama");
}

void Pet::reset(const char* petName, PetVariant petVar) {
    strncpy(name, petName, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
    stage = STAGE_EGG;
    variant = petVar;
    personality = (PersonalityType)(random(0, PERSONALITY_COUNT));

    hunger = DEFAULT_HUNGER;
    happiness = DEFAULT_HAPPY;
    energy = DEFAULT_ENERGY;
    health = DEFAULT_HEALTH;
    weight = DEFAULT_WEIGHT;
    affection = 80;
    ageDays = 0;
    coins = 50;

    isSleeping = false;
    isSick = false;
    isDead = false;
    isDirty = false;
    poopCount = 0;

    totalTimeSeconds = 0;
    lastBathTimeSeconds = 0;
    sickDurationSeconds = 0;
    mealsEaten = 0;
    bathsTaken = 0;
    gamesPlayed = 0;
    medicinesGiven = 0;
    hoursSlept = 0;
    careMistakes = 0;

    timeAccumulator = 0;
    hungerTimer = 0;
    happinessTimer = 0;
    energyTimer = 0;
    healthTimer = 0;
    poopTimer = 0;
    affectionTimer = 0;

    screenTimeoutSec = 60; // 60 segundos por padrão
}

void Pet::update(float dt) {
    if (isDead) return;

    // Acumula tempo em float para evitar perda por truncamento
    timeAccumulator += dt;
    while (timeAccumulator >= 1.0f) {
        timeAccumulator -= 1.0f;
        totalTimeSeconds++;
    }
    ageDays = totalTimeSeconds / 86400; // 1 dia = 86400 segundos

    // Fatores de personalidade
    float hungerMult = 1.0f;
    float happyMult = 1.0f;
    float energyMult = 1.0f;

    switch (personality) {
        case PERSONALITY_GULOSO:     hungerMult = 1.3f; break;
        case PERSONALITY_ALEGRE:     happyMult = 0.8f; break;
        case PERSONALITY_DORMINHOCO: energyMult = 1.3f; break;
        case PERSONALITY_CALMO:      hungerMult = 0.85f; happyMult = 0.85f; energyMult = 0.85f; break;
        case PERSONALITY_BRAVO:      if (hunger < 40) happyMult = 1.4f; break;
        default: break;
    }

    if (stage == STAGE_EGG) {
        // Ovo choca quando totalTimeSeconds atingir 30 segundos
        if (totalTimeSeconds >= 30) {
            stage = STAGE_BABY;
        }
        return;
    }

    // Decaimento de Fome (a cada ~15s em gameplay)
    hungerTimer += dt;
    if (hungerTimer >= (15.0f / hungerMult)) {
        hungerTimer = 0;
        if (!isSleeping) {
            hunger = max(STAT_MIN, hunger - 1);
        }
    }

    // Decaimento de Energia
    energyTimer += dt;
    if (isSleeping) {
        if (energyTimer >= (10.0f / energyMult)) {
            energyTimer = 0;
            energy = min(STAT_MAX, energy + 2);
            if (energy >= STAT_MAX) {
                isSleeping = false; // Acorda automaticamente quando cheio
            }
        }
    } else {
        if (energyTimer >= (20.0f / energyMult)) {
            energyTimer = 0;
            energy = max(STAT_MIN, energy - 1);
        }
    }

    // Decaimento de Felicidade
    happinessTimer += dt;
    if (happinessTimer >= (20.0f / happyMult)) {
        happinessTimer = 0;
        if (hunger < 30 || poopCount > 0 || isSick || isDirty || affection < 20) {
            happiness = max(STAT_MIN, happiness - 2);
        } else if (!isSleeping) {
            happiness = max(STAT_MIN, happiness - 1);
        }
    }

    // Decaimento de Carinho (Affection)
    affectionTimer += dt;
    if (affectionTimer >= 25.0f) {
        affectionTimer = 0;
        if (!isSleeping) {
            affection = max(STAT_MIN, affection - 1);
        }
    }

    // Geração de Cocô (a cada 180s de gameplay se acordado e alimentado)
    if (!isSleeping && hunger > 0) {
        poopTimer += dt;
        if (poopTimer >= 180.0f) {
            poopTimer = 0;
            triggerPoop();
        }
    }

    // Checagem de Banho (Fica sujo se > 300s sem banho no gameplay ou 24h real)
    if (totalTimeSeconds - lastBathTimeSeconds >= 300 && !isDirty) {
        isDirty = true;
    }

    // Saúde e Doenças
    healthTimer += dt;
    if (healthTimer >= 10.0f) {
        healthTimer = 0;
        int healthLoss = 0;
        if (hunger == 0) healthLoss += 2;
        if (energy == 0) healthLoss += 2;
        if (poopCount > 0) healthLoss += poopCount;
        if (isDirty) healthLoss += 1;
        if (isSick) healthLoss += 3;

        if (healthLoss > 0) {
            health = max(STAT_MIN, health - healthLoss);
            careMistakes++;
        }
    }

    // Chance aleatória de ficar doente se o ambiente estiver sujo ou com 4 cocôs
    checkSickness();

    // Acúmulo de tempo de doença (Morre após 24h acumuladas doente)
    if (isSick) {
        sickDurationSeconds += dt;
        if (sickDurationSeconds >= 86400.0f) {
            isDead = true;
        }
    } else {
        sickDurationSeconds = 0;
    }

    // Morte por falta de saúde
    if (health <= 0) {
        isDead = true;
        isSleeping = false;
    }

    // Verificar Evolução
    checkEvolution();
}

void Pet::applyOfflineTime(uint32_t elapsedSeconds) {
    if (isDead) return;
    
    // Limita o tempo offline a no máximo 3 dias (259200s)
    uint32_t capSeconds = min(elapsedSeconds, (uint32_t)259200);
    totalTimeSeconds += capSeconds;
    ageDays = totalTimeSeconds / 86400;

    if (stage == STAGE_EGG) {
        if (totalTimeSeconds >= 30) stage = STAGE_BABY;
        return;
    }

    // Simulação proporcional offline
    int hungerLoss = (capSeconds / 60) * 1; // -1 fome a cada 60s
    int energyLoss = (capSeconds / 120) * 1;
    int happyLoss = (capSeconds / 90) * 1;
    int affectLoss = (capSeconds / 150) * 1;

    hunger = max(STAT_MIN, hunger - hungerLoss);
    energy = max(STAT_MIN, energy - energyLoss);
    happiness = max(STAT_MIN, happiness - happyLoss);
    affection = max(STAT_MIN, affection - affectLoss);

    // Cocô offline (1 a cada 30 min)
    int newPoops = capSeconds / 1800;
    poopCount = min(MAX_POOPS, poopCount + newPoops);

    if (capSeconds >= 86400) {
        isDirty = true;
    }

    if (hunger == 0 || poopCount >= MAX_POOPS || isDirty) {
        isSick = true;
        health = max(0, (int)(health - (capSeconds / 3600) * 5));
    }

    if (isSick) {
        sickDurationSeconds += capSeconds;
        if (sickDurationSeconds >= 86400.0f) { // 24h doente -> morte
            isDead = true;
        }
    }

    if (health <= 0) {
        isDead = true;
    }

    checkEvolution();
}

bool Pet::feed(int hungerGain, int weightGain) {
    if (isDead || isSleeping) return false;
    if (hunger >= STAT_MAX) return false;

    hunger = min(STAT_MAX, hunger + hungerGain);
    weight = min(99, weight + weightGain);
    mealsEaten++;
    return true;
}

bool Pet::play(int happyGain, int energyCost) {
    if (isDead || isSleeping || energy < energyCost) return false;

    happiness = min(STAT_MAX, happiness + happyGain);
    energy = max(STAT_MIN, energy - energyCost);
    weight = max(2, weight - 1);
    gamesPlayed++;
    return true;
}

bool Pet::cure() {
    if (isDead || !isSick) return false;

    isSick = false;
    sickDurationSeconds = 0;
    health = min(STAT_MAX, health + 40);
    medicinesGiven++;
    return true;
}

bool Pet::cleanPoop() {
    if (poopCount <= 0) return false;

    poopCount--; // Limpa apenas UM cocô por vez
    bathsTaken++;
    happiness = min(STAT_MAX, happiness + 5);
    return true;
}

bool Pet::giveBath() {
    if (isDead || isSleeping) return false;

    isDirty = false;
    lastBathTimeSeconds = totalTimeSeconds;
    bathsTaken++;
    happiness = min(STAT_MAX, happiness + 15);
    return true;
}

bool Pet::toggleSleep() {
    if (isDead) return false;
    isSleeping = !isSleeping;
    return true;
}

bool Pet::petCare() {
    if (isDead || isSleeping) return false;
    affection = min(100, affection + 20);
    happiness = min(STAT_MAX, happiness + 10);
    return true;
}

void Pet::addCoins(int amount) {
    coins = min(9999, coins + amount);
}

void Pet::triggerPoop() {
    if (poopCount < MAX_POOPS && stage != STAGE_EGG) {
        poopCount++;
        if (poopCount >= MAX_POOPS) {
            isSick = true;
        }
    }
}

void Pet::checkSickness() {
    if (isSick || isDead || stage == STAGE_EGG) return;

    int sickChance = 0;
    if (poopCount > 0) sickChance += poopCount * 15;
    if (isDirty) sickChance += 25;
    if (health < 30) sickChance += 25;
    if (hunger < 20) sickChance += 20;

    if (sickChance > 0 && random(0, 1000) < sickChance) {
        isSick = true;
    }
}

void Pet::checkEvolution() {
    if (isDead) return;

    // Critérios de Evolução por Idade/Tempo
    if (stage == STAGE_BABY && totalTimeSeconds >= 120) { // Filhote -> Juvenil (2 min em modo demo)
        stage = STAGE_CHILD;
    } else if (stage == STAGE_CHILD && totalTimeSeconds >= 300) { // Juvenil -> Adulto (5 min)
        stage = STAGE_ADULT;
    } else if (stage == STAGE_ADULT && totalTimeSeconds >= 900) { // Adulto -> Senior (15 min)
        stage = STAGE_SENIOR;
    }
}

const char* Pet::getPersonalityName() const {
    switch (personality) {
        case PERSONALITY_ALEGRE:     return "Alegre";
        case PERSONALITY_BRAVO:      return "Bravo";
        case PERSONALITY_DORMINHOCO: return "Dorminhoco";
        case PERSONALITY_GULOSO:     return "Guloso";
        case PERSONALITY_CALMO:      return "Calmo";
        default:                     return "Normal";
    }
}

const char* Pet::getStageName() const {
    switch (stage) {
        case STAGE_EGG:    return "Ovo";
        case STAGE_BABY:   return "Filhote";
        case STAGE_CHILD:  return "Juvenil";
        case STAGE_ADULT:  return "Adulto";
        case STAGE_SENIOR: return "Senior";
        default:           return "Desconhecido";
    }
}

const char* Pet::getStatusText() const {
    if (isDead) return "Morto (X_X)";
    if (isSleeping) return "Dormindo (-.-)zZ";
    if (isSick) return "Doente (@_@)";
    if (isDirty) return "Sujo (S2)";
    if (hunger < 30) return "Com Fome! (Q_Q)";
    if (happiness > 80) return "Muito Feliz! (≧▽≦)";
    if (happiness < 30) return "Triste (T_T)";
    return "Normal (^_^)";
}

