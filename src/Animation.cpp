#include "Animation.h"

Animation::Animation() {
    animTimer = 0;
    currentFrame = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].active = false;
    }
}

void Animation::update(float dt) {
    animTimer += dt;
    if (animTimer >= 0.5f) {
        animTimer = 0;
        currentFrame = (currentFrame + 1) % 4;
    }

    // Atualiza partículas
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].life -= dt * 0.8f;
            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
}

void Animation::spawnParticle(float x, float y, ParticleType type, uint16_t color) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = true;
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = (float)random(-15, 15);
            particles[i].vy = (float)random(-25, -10);
            particles[i].life = 1.0f;
            particles[i].type = type;
            particles[i].color = color;
            break;
        }
    }
}

void Animation::drawParticles(M5Canvas& canvas) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            int px = (int)particles[i].x;
            int py = (int)particles[i].y;
            uint16_t col = particles[i].color;

            if (particles[i].type == PARTICLE_HEART) {
                canvas.fillCircle(px - 2, py - 2, 2, col);
                canvas.fillCircle(px + 2, py - 2, 2, col);
                canvas.fillTriangle(px - 4, py - 1, px + 4, py - 1, px, py + 4, col);
            } else if (particles[i].type == PARTICLE_STAR) {
                canvas.drawPixel(px, py - 3, col);
                canvas.drawPixel(px - 3, py, col);
                canvas.fillRect(px - 1, py - 1, 3, 3, col);
                canvas.drawPixel(px + 3, py, col);
                canvas.drawPixel(px, py + 3, col);
            } else if (particles[i].type == PARTICLE_NOTE) {
                canvas.fillRect(px, py, 2, 6, col);
                canvas.fillCircle(px - 1, py + 6, 2, col);
                canvas.fillRect(px, py, 4, 2, col);
            } else { // SPARKLE
                canvas.drawPixel(px, py, col);
                canvas.drawPixel(px - 1, py, col);
                canvas.drawPixel(px + 1, py, col);
                canvas.drawPixel(px, py - 1, col);
                canvas.drawPixel(px, py + 1, col);
            }
        }
    }
}

void Animation::drawPetSprite(M5Canvas& canvas, int centerX, int centerY, EvolutionStage stage, PetVariant variant, const char* stateText, bool isSleeping, bool isSick, bool isDead, int happyLevel) {
    // Efeito de pulso/respiro suave (bobbing)
    int bounceY = 0;
    if (!isSleeping && !isDead) {
        bounceY = (currentFrame % 2 == 0) ? -2 : 0;
    }
    int cy = centerY + bounceY;

    uint16_t bodyColor = TFT_WHITE;
    uint16_t detailColor = TFT_BLACK;

    switch (variant) {
        case PET_VARIANT_DRAGON: bodyColor = 0x3E0F; break; // Verde Dragão
        case PET_VARIANT_CAT:    bodyColor = 0xFD20; break; // Laranja Gatinho
        default:                 bodyColor = TFT_WHITE; break; // Clássico
    }

    if (isDead) {
        // Lápide / Tombstone
        canvas.fillRoundRect(centerX - 16, cy - 18, 32, 34, 4, TFT_DARKGRAY);
        canvas.drawRoundRect(centerX - 16, cy - 18, 32, 34, 4, TFT_LIGHTGRAY);
        canvas.setTextColor(TFT_WHITE);
        canvas.setTextDatum(MC_DATUM);
        canvas.drawString("R.I.P.", centerX, cy - 6, 2);
        canvas.drawString("X_X", centerX, cy + 8, 2);
        return;
    }

    if (stage == STAGE_EGG) {
        // Ovo com manchas
        canvas.fillEllipse(centerX, cy, 14, 18, bodyColor);
        canvas.drawEllipse(centerX, cy, 14, 18, TFT_BLACK);
        // Manchas
        canvas.fillCircle(centerX - 4, cy - 6, 3, 0xFD20);
        canvas.fillCircle(centerX + 5, cy + 4, 3, 0x07E0);
        canvas.fillCircle(centerX - 3, cy + 8, 2, 0x3E1F);

        if (currentFrame % 2 == 0) {
            canvas.drawLine(centerX - 2, cy - 2, centerX + 2, cy + 2, TFT_BLACK);
        }
        return;
    }

    // Corpo Principal (Base dependendo do estágio)
    int radiusX = 16;
    int radiusY = 16;

    if (stage == STAGE_BABY) {
        radiusX = 14; radiusY = 14;
    } else if (stage == STAGE_CHILD) {
        radiusX = 18; radiusY = 16;
    } else if (stage == STAGE_ADULT) {
        radiusX = 22; radiusY = 20;
    } else if (stage == STAGE_SENIOR) {
        radiusX = 20; radiusY = 18;
    }

    // Orelhas para variações Cat/Dragon
    if (variant == PET_VARIANT_CAT) {
        canvas.fillTriangle(centerX - radiusX + 2, cy - radiusY + 2, centerX - radiusX + 10, cy - radiusY + 2, centerX - radiusX + 4, cy - radiusY - 8, bodyColor);
        canvas.fillTriangle(centerX + radiusX - 2, cy - radiusY + 2, centerX + radiusX - 10, cy - radiusY + 2, centerX + radiusX - 4, cy - radiusY - 8, bodyColor);
        canvas.drawTriangle(centerX - radiusX + 2, cy - radiusY + 2, centerX - radiusX + 10, cy - radiusY + 2, centerX - radiusX + 4, cy - radiusY - 8, TFT_BLACK);
        canvas.drawTriangle(centerX + radiusX - 2, cy - radiusY + 2, centerX + radiusX - 10, cy - radiusY + 2, centerX + radiusX - 4, cy - radiusY - 8, TFT_BLACK);
    } else if (variant == PET_VARIANT_DRAGON) {
        canvas.fillCircle(centerX - radiusX - 4, cy - 2, 5, 0xE105);
        canvas.fillCircle(centerX + radiusX + 4, cy - 2, 5, 0xE105);
    }

    // Desenhar Corpo
    canvas.fillEllipse(centerX, cy, radiusX, radiusY, bodyColor);
    canvas.drawEllipse(centerX, cy, radiusX, radiusY, TFT_BLACK);

    // Patinhas/Pés
    canvas.fillCircle(centerX - radiusX / 2, cy + radiusY - 2, 4, bodyColor);
    canvas.fillCircle(centerX + radiusX / 2, cy + radiusY - 2, 4, bodyColor);
    canvas.drawCircle(centerX - radiusX / 2, cy + radiusY - 2, 4, TFT_BLACK);
    canvas.drawCircle(centerX + radiusX / 2, cy + radiusY - 2, 4, TFT_BLACK);

    // Óculos para Senior
    if (stage == STAGE_SENIOR) {
        canvas.drawCircle(centerX - 7, cy - 4, 5, TFT_BLACK);
        canvas.drawCircle(centerX + 7, cy - 4, 5, TFT_BLACK);
        canvas.drawLine(centerX - 2, cy - 4, centerX + 2, cy - 4, TFT_BLACK);
    }

    // Expressão Facial (Olhos e Boca)
    if (isSleeping) {
        // Olhos fechados (-.-)
        canvas.drawLine(centerX - 9, cy - 4, centerX - 4, cy - 4, TFT_BLACK);
        canvas.drawLine(centerX + 4, cy - 4, centerX + 9, cy - 4, TFT_BLACK);
        // Texto zZ
        canvas.setTextColor(TFT_CYAN);
        canvas.drawString("zZ", centerX + radiusX + 6, cy - radiusY, 2);
    } else if (isSick) {
        // Olhos espirais (@_@)
        canvas.drawCircle(centerX - 6, cy - 4, 3, TFT_BLACK);
        canvas.drawCircle(centerX + 6, cy - 4, 3, TFT_BLACK);
        canvas.drawPixel(centerX - 6, cy - 4, TFT_RED);
        canvas.drawPixel(centerX + 6, cy - 4, TFT_RED);
        // Boca torta
        canvas.drawLine(centerX - 4, cy + 4, centerX + 4, cy + 2, TFT_BLACK);
    } else if (currentFrame == 1) { // Piscar (-_-)
        canvas.drawLine(centerX - 8, cy - 4, centerX - 4, cy - 4, TFT_BLACK);
        canvas.drawLine(centerX + 4, cy - 4, centerX + 8, cy - 4, TFT_BLACK);
        canvas.drawPixel(centerX, cy + 2, TFT_BLACK);
    } else if (happyLevel > 70) { // Happy (≧▽≦)
        canvas.drawLine(centerX - 8, cy - 2, centerX - 5, cy - 6, TFT_BLACK);
        canvas.drawLine(centerX - 5, cy - 6, centerX - 2, cy - 2, TFT_BLACK);
        canvas.drawLine(centerX + 2, cy - 2, centerX + 5, cy - 6, TFT_BLACK);
        canvas.drawLine(centerX + 5, cy - 6, centerX + 8, cy - 2, TFT_BLACK);
        // Boca aberta alegre
        canvas.fillCircle(centerX, cy + 3, 3, 0xE105);
        canvas.drawCircle(centerX, cy + 3, 3, TFT_BLACK);
    } else if (happyLevel < 30) { // Sad (T_T)
        canvas.drawLine(centerX - 6, cy - 6, centerX - 6, cy - 2, TFT_BLACK);
        canvas.drawLine(centerX + 6, cy - 6, centerX + 6, cy - 2, TFT_BLACK);
        canvas.drawLine(centerX - 8, cy - 6, centerX - 4, cy - 6, TFT_BLACK);
        canvas.drawLine(centerX + 4, cy - 6, centerX + 8, cy - 6, TFT_BLACK);
        // Lágrima
        canvas.fillCircle(centerX - 6, cy, 2, TFT_CYAN);
        // Boca triste
        canvas.drawFastHLine(centerX - 4, cy + 4, 8, TFT_BLACK);
    } else { // Normal (^_^)
        canvas.fillCircle(centerX - 6, cy - 4, 2, TFT_BLACK);
        canvas.fillCircle(centerX + 6, cy - 4, 2, TFT_BLACK);
        // Bochechas rosadas
        canvas.fillCircle(centerX - 9, cy, 2, 0xFD20);
        canvas.fillCircle(centerX + 9, cy, 2, 0xFD20);
        // Sorriso
        canvas.drawPixel(centerX - 3, cy + 3, TFT_BLACK);
        canvas.drawPixel(centerX + 3, cy + 3, TFT_BLACK);
        canvas.drawFastHLine(centerX - 2, cy + 4, 5, TFT_BLACK);
    }
}

void Animation::drawIcon(M5Canvas& canvas, int x, int y, int iconIndex, uint16_t color) {
    // Desenha ícones estilizados em pixel art 14x14
    canvas.fillRect(x, y, 14, 14, COLOR_PANEL_BG);
    canvas.drawRect(x, y, 14, 14, color);

    switch (iconIndex) {
        case 0: // 🍖 Alimentar
            canvas.fillCircle(x + 5, y + 5, 3, COLOR_HUNGER);
            canvas.fillCircle(x + 9, y + 9, 3, COLOR_HUNGER);
            canvas.drawLine(x + 3, y + 11, x + 11, y + 3, TFT_WHITE);
            break;
        case 1: // 🎮 Brincar
            canvas.fillRect(x + 2, y + 4, 10, 6, COLOR_HAPPY);
            canvas.drawFastVLine(x + 4, y + 5, 4, TFT_BLACK);
            canvas.drawFastHLine(x + 3, y + 6, 4, TFT_BLACK);
            canvas.drawPixel(x + 9, y + 6, TFT_RED);
            canvas.drawPixel(x + 10, y + 8, TFT_BLUE);
            break;
        case 2: // 💊 Remédio
            canvas.fillRoundRect(x + 3, y + 3, 8, 8, 3, COLOR_HEALTH);
            canvas.drawFastHLine(x + 4, y + 7, 6, TFT_WHITE);
            canvas.drawFastVLine(x + 7, y + 4, 6, TFT_WHITE);
            break;
        case 3: // 💤 Dormir
            canvas.drawString("Z", x + 3, y + 2, 1);
            canvas.drawString("z", x + 8, y + 7, 1);
            break;
        case 4: // 🛁 Banho
            canvas.fillCircle(x + 7, y + 8, 4, TFT_CYAN);
            canvas.fillCircle(x + 5, y + 4, 2, TFT_WHITE);
            canvas.fillCircle(x + 9, y + 5, 2, TFT_WHITE);
            break;
        case 5: // 🧹 Limpar Cocô
            canvas.drawLine(x + 3, y + 3, x + 8, y + 8, 0x8A00);
            canvas.fillTriangle(x + 7, y + 7, x + 12, y + 10, x + 10, y + 12, TFT_YELLOW);
            break;
        case 6: // ❤️ Carinho
            canvas.fillCircle(x + 4, y + 5, 2, COLOR_HEALTH);
            canvas.fillCircle(x + 9, y + 5, 2, COLOR_HEALTH);
            canvas.fillTriangle(x + 2, y + 6, x + 11, y + 6, x + 7, y + 11, COLOR_HEALTH);
            break;
        case 7: // 📊 Status
            canvas.fillRect(x + 3, y + 8, 2, 4, TFT_GREEN);
            canvas.fillRect(x + 6, y + 5, 2, 7, TFT_YELLOW);
            canvas.fillRect(x + 9, y + 2, 2, 10, TFT_RED);
            break;
        case 8: // 🏪 Loja
            canvas.fillTriangle(x + 2, y + 5, x + 12, y + 5, x + 7, y + 2, COLOR_COIN);
            canvas.fillRect(x + 3, y + 6, 8, 6, TFT_LIGHTGRAY);
            canvas.fillRect(x + 6, y + 8, 2, 4, TFT_BLACK);
            break;
        case 9: // 🎒 Inventário
            canvas.fillRoundRect(x + 3, y + 4, 8, 8, 2, 0x9360);
            canvas.drawFastHLine(x + 5, y + 2, 4, TFT_WHITE);
            break;
        case 10: // 🏆 Conquistas
            canvas.fillTriangle(x + 3, y + 3, x + 11, y + 3, x + 7, y + 9, COLOR_COIN);
            canvas.fillRect(x + 6, y + 9, 2, 3, COLOR_COIN);
            canvas.drawFastHLine(x + 4, y + 12, 6, COLOR_COIN);
            break;
        case 11: // 💾 Salvar
            canvas.fillRect(x + 3, y + 3, 8, 8, TFT_BLUE);
            canvas.fillRect(x + 5, y + 3, 4, 3, TFT_WHITE);
            canvas.fillRect(x + 5, y + 8, 4, 3, TFT_YELLOW);
            break;
        case 12: // ⚙️ Config
            canvas.drawCircle(x + 7, y + 7, 3, TFT_LIGHTGRAY);
            canvas.drawPixel(x + 7, y + 3, TFT_LIGHTGRAY);
            canvas.drawPixel(x + 7, y + 11, TFT_LIGHTGRAY);
            canvas.drawPixel(x + 3, y + 7, TFT_LIGHTGRAY);
            canvas.drawPixel(x + 11, y + 7, TFT_LIGHTGRAY);
            break;
        default:
            canvas.fillRect(x + 3, y + 3, 8, 8, color);
            break;
    }
}

void Animation::drawPoop(M5Canvas& canvas, int x, int y) {
    canvas.fillCircle(x + 6, y + 8, 4, COLOR_POOP);
    canvas.fillCircle(x + 6, y + 5, 3, COLOR_POOP);
    canvas.fillCircle(x + 6, y + 2, 2, COLOR_POOP);
    canvas.drawCircle(x + 6, y + 8, 4, TFT_BLACK);
}

void Animation::drawStar(M5Canvas& canvas, int x, int y, uint16_t color) {
    canvas.fillTriangle(x + 6, y, x + 2, y + 10, x + 10, y + 10, color);
    canvas.fillTriangle(x + 6, y + 12, x + 1, y + 4, x + 11, y + 4, color);
}

void Animation::drawBomb(M5Canvas& canvas, int x, int y) {
    canvas.fillCircle(x + 6, y + 7, 5, TFT_BLACK);
    canvas.drawLine(x + 6, y + 2, x + 9, y, TFT_ORANGE);
    canvas.drawPixel(x + 9, y, TFT_YELLOW);
}

void Animation::drawFood(M5Canvas& canvas, int x, int y, int foodType) {
    canvas.fillRect(x, y, 16, 16, COLOR_PANEL_BG);

    switch (foodType) {
        case 0: // 🍎 Maçã
            canvas.fillCircle(x + 8, y + 9, 6, TFT_RED);
            canvas.drawPixel(x + 8, y + 2, 0x03E0);
            canvas.drawPixel(x + 9, y + 3, 0x03E0);
            canvas.drawPixel(x + 7, y + 3, 0x8200);
            break;
        case 1: // 🍖 Hambúrguer
            canvas.fillRoundRect(x + 3, y + 3, 10, 3, 1, 0xFD20); // Pão cima
            canvas.drawFastHLine(x + 3, y + 7, 10, TFT_GREEN); // Alface
            canvas.drawFastHLine(x + 3, y + 8, 10, 0x8A00); // Carne
            canvas.fillRoundRect(x + 3, y + 9, 10, 3, 1, 0xFD20); // Pão baixo
            break;
        case 2: // 🍰 Bolo
            canvas.fillRect(x + 3, y + 6, 10, 7, 0xFC18); // Bolo
            canvas.drawFastHLine(x + 3, y + 6, 10, TFT_WHITE); // Cobertura
            canvas.fillCircle(x + 8, y + 4, 2, TFT_RED); // Cereja
            break;
        case 3: // 🥗 Salada
            canvas.fillRoundRect(x + 2, y + 8, 12, 5, 2, TFT_WHITE); // Tigela
            canvas.fillCircle(x + 6, y + 6, 3, TFT_GREEN);
            canvas.fillCircle(x + 10, y + 6, 3, TFT_RED);
            break;
        case 4: // 💊 Vitamina (Cápusla)
            canvas.fillRoundRect(x + 4, y + 3, 8, 10, 4, TFT_RED);
            canvas.fillRect(x + 4, y + 8, 8, 5, TFT_WHITE);
            canvas.drawRoundRect(x + 4, y + 3, 8, 10, 4, TFT_BLACK);
            break;
        case 5: // 🩹 Xarope
            canvas.fillRect(x + 5, y + 3, 6, 10, 0x07FF); // Frasco
            canvas.fillRect(x + 6, y + 1, 4, 2, TFT_WHITE); // Tampa
            canvas.drawFastHLine(x + 6, y + 7, 4, TFT_RED);
            canvas.drawFastVLine(x + 7, y + 6, 3, TFT_RED);
            break;
        case 6: // 💉 Injeção
            canvas.drawLine(x + 2, y + 13, x + 6, y + 9, TFT_LIGHTGRAY); // Agulha
            canvas.fillRect(x + 6, y + 5, 6, 5, TFT_WHITE); // Seringa
            canvas.drawRect(x + 6, y + 5, 6, 5, TFT_BLACK);
            canvas.drawLine(x + 11, y + 4, x + 14, y + 1, TFT_BLACK); // Êmbolo
            break;
        case 7: // ⚽ Bola
            canvas.fillCircle(x + 8, y + 8, 6, TFT_WHITE);
            canvas.drawCircle(x + 8, y + 8, 6, TFT_BLACK);
            canvas.fillRect(x + 7, y + 7, 3, 3, TFT_BLACK);
            break;
        case 8: // 🎮 GameBoy
            canvas.fillRoundRect(x + 4, y + 2, 8, 12, 2, TFT_LIGHTGRAY);
            canvas.fillRect(x + 6, y + 4, 4, 3, TFT_BLACK); // Tela
            canvas.drawPixel(x + 6, y + 9, TFT_DARKGRAY); // D-Pad
            canvas.drawPixel(x + 9, y + 10, TFT_RED); // Botão A
            break;
        case 9: // 🧽 Planta
            canvas.fillTriangle(x + 4, y + 13, x + 12, y + 13, x + 8, y + 8, 0x8A00); // Vaso
            canvas.fillCircle(x + 6, y + 6, 3, TFT_GREEN); // Folha esq
            canvas.fillCircle(x + 10, y + 5, 3, TFT_GREEN); // Folha dir
            break;
        case 10: // 🪣 Tapete
            canvas.fillRoundRect(x + 2, y + 5, 12, 7, 2, 0x3E1F);
            canvas.drawRoundRect(x + 2, y + 5, 12, 7, 2, COLOR_COIN);
            break;
        case 11: // 🛏 Quadro
            canvas.drawRect(x + 3, y + 3, 10, 10, COLOR_COIN);
            canvas.fillRect(x + 4, y + 4, 8, 8, COLOR_BG_DAY);
            canvas.fillTriangle(x + 5, y + 10, x + 11, y + 10, x + 8, y + 6, TFT_GREEN); // Montanha
            break;
        default:
            canvas.fillCircle(x + 8, y + 8, 5, COLOR_HUNGER);
            break;
    }
}

