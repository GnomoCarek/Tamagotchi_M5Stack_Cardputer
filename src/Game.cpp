#include "Game.h"

Game::Game() {
    currentState = STATE_TITLE;
    lastStage = STAGE_EGG;
    brightness = 128;
    lastFrameTime = 0;
    autoSaveTimer = 0;
    hatchingTimer = 0;

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
        if (storage.loadGame(pet, items, achievements, sound, clock, brightness, offlineSecs)) {
            M5Cardputer.Display.setBrightness(brightness);
            lastStage = pet.stage;

            currentState = STATE_INITIAL_MENU; // Apresenta menu Continuar/Novo Jogo/Config

            if (offlineSecs > 0) {
                pet.applyOfflineTime(offlineSecs);
                char offlineMsg[48];
                snprintf(offlineMsg, sizeof(offlineMsg), "Bem-vindo! Passaram-se %ds", offlineSecs);
                events.showMessage(offlineMsg, 4.0f);
            }
        }
    } else {
        pet.reset("TamaCard", GENDER_MALE);
        lastStage = pet.stage;
        currentState = STATE_TITLE;
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
            return;
        }
    } else {
        screenIdleTimer += dt;
        if (pet.screenTimeoutSec > 0 && screenIdleTimer >= (float)pet.screenTimeoutSec && !isScreenOff) {
            isScreenOff = true;
            M5Cardputer.Display.setBrightness(0);
            sound.playSound(SOUND_SCREEN_SLEEP);
        }
    }

    if (isScreenOff) {
        pet.update(dt);
        clock.update(dt);
        sound.update(dt);
        return;
    }

    // Atualizar Som & Relógio & Animações
    sound.update(dt);
    clock.update(dt);
    animation.update(dt);

    // Auto-Save periódico a cada 30s
    if (currentState == STATE_GAMEPLAY || currentState == STATE_MENU) {
        autoSaveTimer += dt;
        if (autoSaveTimer >= 30.0f) {
            autoSaveTimer = 0;
            saveGame();
        }
    }

    // Máquina de Estados do Jogo
    switch (currentState) {
        case STATE_TITLE:
            renderer.renderTitleScreen(animation);
            if (action == ACTION_SELECT) {
                sound.playSound(SOUND_CLICK);
                if (storage.hasSaveData()) {
                    currentState = STATE_INITIAL_MENU;
                } else {
                    currentState = STATE_GENDER_ENTRY;
                }
            }
            break;

        case STATE_INITIAL_MENU: {
            bool dummySave = false, dummyReset = false;
            menu.update(action, typedC, pet, items, achievements, sound, clock, brightness, dummySave, dummyReset, currentState);
            menu.draw(renderer.getCanvas(), pet, items, achievements, sound, clock, animation, brightness, currentState);
            break;
        }

        case STATE_GENDER_ENTRY: {
            bool dummySave = false, dummyReset = false;
            menu.update(action, typedC, pet, items, achievements, sound, clock, brightness, dummySave, dummyReset, currentState);
            renderer.renderGenderEntryScreen(menu.getSelectedGender(), animation);
            break;
        }

        case STATE_NAME_ENTRY:
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

            renderer.renderNameEntryScreen(nameInput, menu.getSelectedGender(), animation);

            if (action == ACTION_SELECT) {
                if (nameLen >= 3) {
                    pet.reset(nameInput, menu.getSelectedGender());
                    lastStage = pet.stage;
                    saveGame();
                    hatchingTimer = 0;
                    currentState = STATE_HATCHING;
                    sound.playSound(SOUND_EGG_HATCH);
                } else {
                    events.showMessage("Nome deve ter 3-12 letras!", 2.5f);
                }
            }
            break;

        case STATE_HATCHING:
            hatchingTimer += dt;
            renderer.renderHatchingScreen(hatchingTimer, animation);

            if (hatchingTimer >= 3.5f || action == ACTION_SELECT) {
                currentState = STATE_GAMEPLAY;
                sound.playSound(SOUND_EGG_HATCH);
            }
            break;

        case STATE_MINIGAME_SELECT: {
            bool dummySave = false, dummyReset = false;
            menu.update(action, typedC, pet, items, achievements, sound, clock, brightness, dummySave, dummyReset, currentState);
            menu.draw(renderer.getCanvas(), pet, items, achievements, sound, clock, animation, brightness, currentState);
            break;
        }

        case STATE_MINIGAME_CATCH:
            if (minigame.getType() != MINIGAME_CATCH_STARS) {
                minigame.reset(MINIGAME_CATCH_STARS);
            }
            minigame.update(dt, action, typedC, sound, animation);
            minigame.draw(renderer.getCanvas(), animation);
            renderer.getCanvas().pushSprite(0, 0);

            if (minigame.isGameOver() && action == ACTION_SELECT) {
                pet.addCoins(minigame.getCoinsEarned());
                pet.play(15, 10);
                currentState = STATE_GAMEPLAY;
                sound.playSound(SOUND_CLICK);
            }
            break;

        case STATE_MINIGAME_REFLEX:
            if (minigame.getType() != MINIGAME_REFLEX_TEST) {
                minigame.reset(MINIGAME_REFLEX_TEST);
            }
            minigame.update(dt, action, typedC, sound, animation);
            minigame.draw(renderer.getCanvas(), animation);
            renderer.getCanvas().pushSprite(0, 0);

            if (minigame.isGameOver() && action == ACTION_SELECT) {
                pet.addCoins(minigame.getCoinsEarned());
                pet.play(25, 10);
                currentState = STATE_GAMEPLAY;
                sound.playSound(SOUND_CLICK);
            }
            break;

        case STATE_GAMEPLAY:
        case STATE_MENU: {
            pet.update(dt);
            events.update(dt, pet, sound);
            achievements.checkProgress(pet, minigame.getScore());

            if (pet.stage != lastStage) {
                lastStage = pet.stage;
                currentState = STATE_EVOLUTION;
                sound.playSound(SOUND_EVOLUTION);
                animation.spawnParticle(SCREEN_WIDTH / 2, 70, PARTICLE_STAR, COLOR_COIN);
                return;
            }

            if (pet.isDead) {
                currentState = STATE_GAME_OVER;
                sound.playSound(SOUND_DEATH);
                return;
            }

            if (currentState == STATE_GAMEPLAY) {
                if (action == ACTION_SELECT) {
                    currentState = STATE_MENU;
                    sound.playSound(SOUND_CLICK);
                } else if (action == ACTION_NUM1 || action == ACTION_SPACE) {
                    if (pet.petCare()) {
                        sound.playSound(SOUND_PLAY);
                        animation.spawnParticle(SCREEN_WIDTH / 2, 60, PARTICLE_HEART, COLOR_HEALTH);
                    }
                } else if (action == ACTION_NUM2) {
                    if (pet.cleanPoop()) sound.playSound(SOUND_CLEAN);
                } else if (action == ACTION_NUM3) {
                    if (pet.giveBath()) sound.playSound(SOUND_CLEAN);
                }
            } else if (currentState == STATE_MENU) {
                bool requestSave = false;
                bool requestReset = false;
                menu.update(action, typedC, pet, items, achievements, sound, clock, brightness, requestSave, requestReset, currentState);

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

            renderer.renderGameplay(pet, clock, animation, events, menu, items, achievements, sound, currentState, brightness);
            break;
        }

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
                currentState = STATE_GENDER_ENTRY;
                sound.playSound(SOUND_CLICK);
            }
            break;

        default:
            currentState = STATE_GAMEPLAY;
            break;
    }
}

void Game::saveGame() {
    storage.saveGame(pet, items, achievements, sound, clock, brightness);
}

void Game::resetGame() {
    storage.resetSave();
    pet.reset("TamaCard", GENDER_MALE);
    items = ItemsManager();
    achievements = AchievementsManager();
    minigame.reset(MINIGAME_CATCH_STARS);
    lastStage = pet.stage;
}

