#include "MiniGame.h"

MiniGame::MiniGame() {
    reset();
}

void MiniGame::reset() {
    playerX = SCREEN_WIDTH / 2.0f;
    score = 0;
    coinsEarned = 0;
    lives = 3;
    gameOver = false;
    spawnTimer = 0;

    for (int i = 0; i < MAX_FALLING_ITEMS; i++) {
        items[i].active = false;
    }
}

void MiniGame::update(float dt, KeyAction action, Sound& sound, Animation& anim) {
    if (gameOver) return;

    // Movimento do Jogador (Cesto)
    float speed = 120.0f * dt;
    if (action == ACTION_LEFT) {
        playerX = max(15.0f, playerX - speed);
    } else if (action == ACTION_RIGHT) {
        playerX = min((float)(SCREEN_WIDTH - 15), playerX + speed);
    }

    // Spawning de Itens
    spawnTimer += dt;
    if (spawnTimer >= 0.8f) {
        spawnTimer = 0;
        for (int i = 0; i < MAX_FALLING_ITEMS; i++) {
            if (!items[i].active) {
                items[i].active = true;
                items[i].x = (float)random(10, SCREEN_WIDTH - 10);
                items[i].y = -10;
                items[i].speed = (float)random(40, 80);
                items[i].isStar = (random(0, 100) < 75); // 75% estrela, 25% bomba
                break;
            }
        }
    }

    // Atualiza Itens em queda
    for (int i = 0; i < MAX_FALLING_ITEMS; i++) {
        if (items[i].active) {
            items[i].y += items[i].speed * dt;

            // Colisão com Cesto (y ~ 118)
            if (items[i].y >= 114.0f && items[i].y <= 126.0f) {
                if (abs(items[i].x - playerX) <= 16.0f) {
                    items[i].active = false;
                    if (items[i].isStar) {
                        score += 10;
                        coinsEarned += 2;
                        sound.playSound(SOUND_COIN);
                        anim.spawnParticle(items[i].x, items[i].y, PARTICLE_STAR, COLOR_COIN);
                    } else { // Bomba
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

            // Saiu da tela
            if (items[i].y > SCREEN_HEIGHT + 10) {
                items[i].active = false;
            }
        }
    }
}

void MiniGame::draw(M5Canvas& canvas, Animation& anim) {
    // Fundo Noite Estrelada
    canvas.fillScreen(0x084A);

    // Título e Status
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(TL_DATUM);
    canvas.drawString("CAPTURAR ESTRELAS", 8, 4);

    char scoreStr[32];
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

    // Tela de Fim de Jogo
    if (gameOver) {
        canvas.fillRoundRect(30, 35, 180, 65, 6, COLOR_PANEL_BG);
        canvas.drawRoundRect(30, 35, 180, 65, 6, TFT_WHITE);
        canvas.setTextDatum(MC_DATUM);
        canvas.setTextColor(TFT_YELLOW);
        canvas.drawString("FIM DE JOGO!", 120, 52);
        canvas.setTextColor(TFT_WHITE);
        snprintf(scoreStr, sizeof(scoreStr), "Ganhou %d Moedas!", coinsEarned);
        canvas.drawString(scoreStr, 120, 72);
        canvas.drawString("[ENTER] Sair", 120, 88);
    }
}
