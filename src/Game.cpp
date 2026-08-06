#include "Game.h"

Game::Game() {
    currentState = STATE_TITLE;
    lastStage = STAGE_EGG;
    timeSpeed = 1.0f;
    brightness = 128;
    lastFrameTime = 0;
    autoSaveTimer = 0;
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
                currentState = STATE_GAMEPLAY;
                sound.playSound(SOUND_CLICK);
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
                } else if (action == ACTION_NUM1) { // Atalho Carinho
                    if (pet.petCare()) {
                        sound.playSound(SOUND_PLAY);
                        animation.spawnParticle(SCREEN_WIDTH / 2, 60, PARTICLE_HEART, COLOR_HEALTH);
                    }
                } else if (action == ACTION_NUM2) { // Atalho Limpar
                    if (pet.cleanPoop()) sound.playSound(SOUND_CLEAN);
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
                currentState = STATE_TITLE;
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
