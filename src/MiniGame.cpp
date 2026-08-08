#include "MiniGame.h"

MiniGame::MiniGame() {
    reset(MINIGAME_CATCH_STARS);
}

void MiniGame::reset(MiniGameType type) {
    currentGameType = type;
    score = 0;
    coinsEarned = 0;
    lives = 3;
    gameOver = false;

    // Reset Capturar Estrelas
    playerX = SCREEN_WIDTH / 2.0f;
    spawnTimer = 0;
    for (int i = 0; i < MAX_FALLING_ITEMS; i++) {
        items[i].active = false;
    }

    // Reset Teste de Reflexo
    comboStreak = 0;
    maxCombo = 0;
    reflexFeedbackTimer = 0;
    reflexSuccess = false;

    if (currentGameType == MINIGAME_REFLEX_TEST) {
        spawnReflexTarget();
    }
}

void MiniGame::spawnReflexTarget() {
    int pick = random(0, 10);
    targetChar = '\0';
    targetAction = ACTION_NONE;

    switch (pick) {
        case 0: targetName = "TECLA A"; targetAction = ACTION_LEFT; targetChar = 'a'; break;
        case 1: targetName = "TECLA D"; targetAction = ACTION_RIGHT; targetChar = 'd'; break;
        case 2: targetName = "TECLA W"; targetAction = ACTION_UP; targetChar = 'w'; break;
        case 3: targetName = "TECLA S"; targetAction = ACTION_DOWN; targetChar = 's'; break;
        case 4: targetName = "TECLA 1"; targetAction = ACTION_NUM1; targetChar = '1'; break;
        case 5: targetName = "TECLA 2"; targetAction = ACTION_NUM2; targetChar = '2'; break;
        case 6: targetName = "TECLA 3"; targetAction = ACTION_NUM3; targetChar = '3'; break;
        case 7: targetName = "TECLA 4"; targetAction = ACTION_NUM4; targetChar = '4'; break;
        case 8: targetName = "ENTER";   targetAction = ACTION_SELECT; break;
        case 9: targetName = "ESPACO";  targetAction = ACTION_SPACE; break;
        default: targetName = "TECLA A"; targetAction = ACTION_LEFT; targetChar = 'a'; break;
    }

    // Dificuldade progressiva: tempo limite diminui conforme o combo aumenta
    timeLimit = max(0.65f, 1.8f - (comboStreak * 0.06f));
    timeRemaining = timeLimit;
}

void MiniGame::update(float dt, KeyAction action, char typedChar, Sound& sound, Animation& anim) {
    if (gameOver) return;

    if (currentGameType == MINIGAME_CATCH_STARS) {
        // =========================================
        // MINIJOGO 1: CAPTURAR ESTRELAS
        // =========================================
        float speed = 120.0f * dt;
        if (action == ACTION_LEFT) {
            playerX = max(15.0f, playerX - speed);
        } else if (action == ACTION_RIGHT) {
            playerX = min((float)(SCREEN_WIDTH - 15), playerX + speed);
        }

        spawnTimer += dt;
        if (spawnTimer >= 0.8f) {
            spawnTimer = 0;
            for (int i = 0; i < MAX_FALLING_ITEMS; i++) {
                if (!items[i].active) {
                    items[i].active = true;
                    items[i].x = (float)random(10, SCREEN_WIDTH - 10);
                    items[i].y = -10;
                    items[i].speed = (float)random(40, 80);
                    items[i].isStar = (random(0, 100) < 75);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_FALLING_ITEMS; i++) {
            if (items[i].active) {
                items[i].y += items[i].speed * dt;

                if (items[i].y >= 114.0f && items[i].y <= 126.0f) {
                    if (abs(items[i].x - playerX) <= 16.0f) {
                        items[i].active = false;
                        if (items[i].isStar) {
                            score += 10;
                            coinsEarned += 2;
                            sound.playSound(SOUND_COIN);
                            anim.spawnParticle(items[i].x, items[i].y, PARTICLE_STAR, COLOR_COIN);
                        } else {
                            lives--;
                            sound.playSound(SOUND_SICK);
                            anim.spawnParticle(items[i].x, items[i].y, PARTICLE_SPARKLE, TFT_RED);
                            if (lives <= 0) {
                                gameOver = true;
                                sound.playSound(SOUND_DEATH);
                            }
                        }
                    }
                }

                if (items[i].y > SCREEN_HEIGHT + 10) {
                    items[i].active = false;
                }
            }
        }

    } else if (currentGameType == MINIGAME_REFLEX_TEST) {
        // =========================================
        // MINIJOGO 2: TESTE DE REFLEXO
        // =========================================
        if (reflexFeedbackTimer > 0) {
            reflexFeedbackTimer -= dt;
        }

        timeRemaining -= dt;

        bool hit = false;
        bool wrong = false;

        // Checar entrada do jogador
        if (action != ACTION_NONE || typedChar != '\0') {
            bool matchesAction = (targetAction != ACTION_NONE && action == targetAction);
            bool matchesChar = (targetChar != '\0' && (tolower(typedChar) == tolower(targetChar)));

            if (matchesAction || matchesChar) {
                hit = true;
            } else {
                wrong = true;
            }
        }

        if (hit) {
            comboStreak++;
            if (comboStreak > maxCombo) maxCombo = comboStreak;
            int reward = 10 * comboStreak;
            score += reward;
            coinsEarned += 2 + (comboStreak / 3);

            sound.playSound(SOUND_CORRECT);
            anim.spawnParticle(SCREEN_WIDTH / 2, 60, PARTICLE_STAR, COLOR_COIN);

            reflexSuccess = true;
            reflexFeedbackTimer = 0.25f;
            spawnReflexTarget();
        } else if (wrong || timeRemaining <= 0) {
            lives--;
            comboStreak = 0;

            sound.playSound(SOUND_WRONG);
            anim.spawnParticle(SCREEN_WIDTH / 2, 60, PARTICLE_SPARKLE, TFT_RED);

            reflexSuccess = false;
            reflexFeedbackTimer = 0.35f;

            if (lives <= 0) {
                gameOver = true;
                sound.playSound(SOUND_DEATH);
            } else {
                spawnReflexTarget();
            }
        }
    }
}

void MiniGame::draw(M5Canvas& canvas, Animation& anim) {
    if (currentGameType == MINIGAME_CATCH_STARS) {
        // Fundo Noite Estrelada
        canvas.fillScreen(0x084A);

        canvas.setTextColor(TFT_WHITE);
        canvas.setTextDatum(TL_DATUM);
        canvas.drawString("CAPTURAR ESTRELAS", 8, 4);

        char scoreStr[48];
        snprintf(scoreStr, sizeof(scoreStr), "Score: %d  Moedas: +%d", score, coinsEarned);
        canvas.drawString(scoreStr, 8, 22);

        // Vidas
        for (int l = 0; l < lives; l++) {
            canvas.fillCircle(SCREEN_WIDTH - 15 - (l * 12), 10, 4, COLOR_HEALTH);
        }

        // Desenhar Itens em Queda
        for (int i = 0; i < MAX_FALLING_ITEMS; i++) {
            if (items[i].active) {
                if (items[i].isStar) {
                    anim.drawStar(canvas, (int)items[i].x - 6, (int)items[i].y - 6, COLOR_COIN);
                } else {
                    anim.drawBomb(canvas, (int)items[i].x - 6, (int)items[i].y - 6);
                }
            }
        }

        // Desenhar Cesto do Jogador
        int px = (int)playerX;
        canvas.fillRoundRect(px - 14, 118, 28, 10, 3, 0xFD20);
        canvas.drawRoundRect(px - 14, 118, 28, 10, 3, TFT_BLACK);

    } else if (currentGameType == MINIGAME_REFLEX_TEST) {
        // Fundo Roxo / Ciano Arcade
        canvas.fillScreen(0x18C6);

        canvas.setTextColor(TFT_WHITE);
        canvas.setTextDatum(TL_DATUM);
        canvas.drawString("TESTE DE REFLEXO", 8, 4);

        char scoreStr[64];
        snprintf(scoreStr, sizeof(scoreStr), "Score: %d | Combo: x%d | +%dC", score, comboStreak, coinsEarned);
        canvas.drawString(scoreStr, 8, 22);

        // Vidas
        for (int l = 0; l < lives; l++) {
            canvas.fillCircle(SCREEN_WIDTH - 15 - (l * 12), 10, 4, COLOR_HEALTH);
        }

        // Barra de Tempo Dinâmica
        int barW = 200;
        int barH = 10;
        int barX = (SCREEN_WIDTH - barW) / 2;
        int barY = 38;

        float ratio = constrain(timeRemaining / timeLimit, 0.0f, 1.0f);
        uint16_t barCol = (ratio > 0.5f) ? COLOR_ENERGY : ((ratio > 0.25f) ? COLOR_HUNGER : COLOR_HEALTH);

        canvas.fillRoundRect(barX, barY, barW, barH, 3, COLOR_BAR_BG);
        canvas.fillRoundRect(barX, barY, (int)(barW * ratio), barH, 3, barCol);
        canvas.drawRoundRect(barX, barY, barW, barH, 3, TFT_WHITE);

        // Moldura Central da Tecla Alvo
        int boxW = 140;
        int boxH = 46;
        int boxX = (SCREEN_WIDTH - boxW) / 2;
        int boxY = 56;

        uint16_t boxCol = (reflexFeedbackTimer > 0) ? (reflexSuccess ? COLOR_ENERGY : COLOR_HEALTH) : COLOR_ACCENT;
        canvas.fillRoundRect(boxX, boxY, boxW, boxH, 6, COLOR_PANEL_BG);
        canvas.drawRoundRect(boxX, boxY, boxW, boxH, 6, boxCol);
        canvas.drawRoundRect(boxX + 1, boxY + 1, boxW - 2, boxH - 2, 5, boxCol);

        // Texto da Tecla Alvo em destaque
        canvas.setTextColor(TFT_YELLOW);
        canvas.setTextDatum(MC_DATUM);
        canvas.drawString(targetName, SCREEN_WIDTH / 2, boxY + 23);

        canvas.setTextColor(TFT_LIGHTGRAY);
        canvas.drawString("Pressione rapido!", SCREEN_WIDTH / 2, 114);
    }

    // Tela de Fim de Jogo
    if (gameOver) {
        canvas.fillRoundRect(30, 35, 180, 65, 6, COLOR_PANEL_BG);
        canvas.drawRoundRect(30, 35, 180, 65, 6, TFT_WHITE);
        canvas.setTextDatum(MC_DATUM);
        canvas.setTextColor(TFT_YELLOW);
        canvas.drawString("FIM DE JOGO!", 120, 52);
        canvas.setTextColor(TFT_WHITE);
        char scoreStr[48];
        snprintf(scoreStr, sizeof(scoreStr), "Ganhou %d Moedas!", coinsEarned);
        canvas.drawString(scoreStr, 120, 72);
        canvas.drawString("[ENTER] Sair", 120, 88);
    }
}
