#include "Game.h"

Game::Game() {
    currentState = STATE_TITLE;
    lastStage = STAGE_EGG;
    timeSpeed = 1.0f;
    brightness = 128;
    lastFrameTime = 0;
    autoSaveTimer = 0;

    nameInput[0] = '\0';
    nameLen = 0;
    screenIdleTimer = 0;
    isScreenOff = false;
}

void Game::begin() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);

    renderer.begin();
    sound.begin();
    storage.begin();

    M5Cardputer.Display.setBrightness(brightness);

    // Carregar Jogo se Existir Save
    uint32_t offlineSecs = 0;
    if (storage.hasSaveData()) {
        if (storage.loadGame(pet, items, achievements, sound, timeSpeed, brightness, offlineSecs)) {
            M5Cardputer.Display.setBrightness(brightness);
            lastStage = pet.stage;

            if (offlineSecs > 0) {
                pet.applyOfflineTime(offlineSecs);
                char offlineMsg[48];
                snprintf(offlineMsg, sizeof(offlineMsg), "Bem-vindo! Passaram-se %ds", offlineSecs);
                events.showMessage(offlineMsg, 4.0f);
            }
        }
    } else {
        pet.reset("TamaCard");
        lastStage = pet.stage;
    }

    sound.playSound(SOUND_BOOT);
    lastFrameTime = millis();
}

void Game::update() {
    uint32_t now = millis();
    float dt = (now - lastFrameTime) / 1000.0f;
    if (dt <= 0) dt = 0.001f;
    if (dt > 0.1f) dt = 0.1f; // Clamp delta time
    lastFrameTime = now;

    // Atualizar Entrada
    input.update();
    KeyAction action = input.getAction();
    char typedC = input.getTypedChar();
    bool anyInput = (action != ACTION_NONE || typedC != '\0');

    // Gerenciamento de Timeout da Tela (Sleep / Wake)
    if (anyInput) {
        screenIdleTimer = 0;
        if (isScreenOff) {
            isScreenOff = false;
            M5Cardputer.Display.setBrightness(brightness);
            sound.playSound(SOUND_SCREEN_WAKE);
            return; // Descarta o toque inicial que religou a tela
        }
    } else {
        screenIdleTimer += dt;
        if (pet.screenTimeoutSec > 0 && screenIdleTimer >= (float)pet.screenTimeoutSec && !isScreenOff) {
            isScreenOff = true;
            M5Cardputer.Display.setBrightness(0);
            sound.playSound(SOUND_SCREEN_SLEEP);
        }
    }

    // Se a tela estiver desligada, mantém a simulação sem renderizar
    if (isScreenOff) {
        pet.update(dt);
        clock.setTimeSpeed(timeSpeed);
        clock.update(dt);
        sound.update(dt);
        return;
    }

    // Atualizar Som & Relógio
    sound.update(dt);
    clock.setTimeSpeed(timeSpeed);
    clock.update(dt);
    animation.update(dt);

    // Auto-Save periódico a cada 30s
    autoSaveTimer += dt;
    if (autoSaveTimer >= 30.0f) {
        autoSaveTimer = 0;
        saveGame();
    }

    // Máquina de Estados do Jogo
    switch (currentState) {
        case STATE_TITLE:
            renderer.renderTitleScreen(animation);
            if (action == ACTION_SELECT) {
                sound.playSound(SOUND_CLICK);
                if (storage.hasSaveData()) {
                    currentState = STATE_GAMEPLAY;
                } else {
                    nameInput[0] = '\0';
                    nameLen = 0;
                    currentState = STATE_NAME_ENTRY;
                }
            }
            break;

        case STATE_NAME_ENTRY:
            // Digitação do Nome
            if (typedC == '\b' || action == ACTION_DELETE) {
                if (nameLen > 0) {
                    nameLen--;
                    nameInput[nameLen] = '\0';
                    sound.playSound(SOUND_CLICK);
                }
            } else if (typedC >= ' ' && typedC <= '~' && nameLen < 12) {
                nameInput[nameLen] = typedC;
                nameLen++;
                nameInput[nameLen] = '\0';
                sound.playSound(SOUND_TYPING);
            }

            renderer.renderNameEntryScreen(nameInput, animation);

            if (action == ACTION_SELECT) {
                if (nameLen >= 3) {
                    pet.reset(nameInput);
                    lastStage = pet.stage;
                    saveGame();
                    currentState = STATE_GAMEPLAY;
                    sound.playSound(SOUND_EGG_HATCH);
                } else {
                    events.showMessage("Nome deve ter 3-12 letras!", 2.5f);
                }
            }
            break;

        case STATE_GAMEPLAY:
        case STATE_MENU: {
            pet.update(dt);
            events.update(dt, pet, sound);
            achievements.checkProgress(pet, minigame.getScore());

            // Detecção de Evolução
            if (pet.stage != lastStage) {
                lastStage = pet.stage;
                currentState = STATE_EVOLUTION;
                sound.playSound(SOUND_EVOLUTION);
                animation.spawnParticle(SCREEN_WIDTH / 2, 70, PARTICLE_STAR, COLOR_COIN);
                return;
            }

            // Morte
            if (pet.isDead) {
                currentState = STATE_GAME_OVER;
                sound.playSound(SOUND_DEATH);
                return;
            }

            // Tratar Controles em Gameplay Livre
            if (currentState == STATE_GAMEPLAY) {
                if (action == ACTION_SELECT) {
                    currentState = STATE_MENU;
                    sound.playSound(SOUND_CLICK);
                } else if (action == ACTION_NUM1 || action == ACTION_SPACE) { // Atalho Carinho
                    if (pet.petCare()) {
                        sound.playSound(SOUND_PLAY);
                        animation.spawnParticle(SCREEN_WIDTH / 2, 60, PARTICLE_HEART, COLOR_HEALTH);
                    }
                } else if (action == ACTION_NUM2) { // Atalho Limpar
                    if (pet.cleanPoop()) sound.playSound(SOUND_CLEAN);
                } else if (action == ACTION_NUM3) { // Atalho Banho
                    if (pet.giveBath()) sound.playSound(SOUND_CLEAN);
                }
            } else if (currentState == STATE_MENU) {
                bool requestSave = false;
                bool requestReset = false;
                menu.update(action, pet, items, achievements, sound, timeSpeed, brightness, requestSave, requestReset, currentState);

                if (requestSave) {
                    saveGame();
                    events.showMessage("Jogo Salvo com Sucesso! 💾", 3.0f);
                    sound.playSound(SOUND_ACHIEVEMENT);
                    menu.closeSubMenu();
                }
                if (requestReset) {
                    resetGame();
                    currentState = STATE_TITLE;
                    return;
                }
            }

            renderer.renderGameplay(pet, clock, animation, events, menu, items, achievements, sound, currentState, timeSpeed, brightness);
            break;
        }

        case STATE_MINIGAME:
            minigame.update(dt, action, sound, animation);
            minigame.draw(renderer.getCanvas(), animation);
            renderer.getCanvas().pushSprite(0, 0);

            if (minigame.isGameOver() && action == ACTION_SELECT) {
                pet.addCoins(minigame.getCoinsEarned());
                pet.play(15, 10); // Minijogo aumenta felicidade e gasta energia
                minigame.reset();
                currentState = STATE_GAMEPLAY;
                sound.playSound(SOUND_CLICK);
            }
            break;

        case STATE_EVOLUTION:
            renderer.renderEvolutionScreen(pet, animation);
            if (action == ACTION_SELECT) {
                currentState = STATE_GAMEPLAY;
                sound.playSound(SOUND_CLICK);
            }
            break;

        case STATE_GAME_OVER:
            renderer.renderGameOverScreen(pet, animation);
            if (action == ACTION_SELECT) {
                resetGame();
                nameInput[0] = '\0';
                nameLen = 0;
                currentState = STATE_NAME_ENTRY;
                sound.playSound(SOUND_CLICK);
            }
            break;

        default:
            currentState = STATE_GAMEPLAY;
            break;
    }
}

void Game::saveGame() {
    storage.saveGame(pet, items, achievements, sound, timeSpeed, brightness);
}

void Game::resetGame() {
    storage.resetSave();
    pet.reset("TamaCard");
    items = ItemsManager();
    achievements = AchievementsManager();
    minigame.reset();
    lastStage = pet.stage;
}

