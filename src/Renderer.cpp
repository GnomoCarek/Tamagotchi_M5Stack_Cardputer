#include "Renderer.h"
#include <M5Unified.h>

Renderer::Renderer() : canvas(&M5Cardputer.Display) {
    initialized = false;
}

void Renderer::begin() {
    canvas.setColorDepth(16);
    canvas.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    initialized = true;
}

void Renderer::renderGameplay(const Pet& pet, const Clock& clock, Animation& anim, EventsManager& events, MenuManager& menu, const ItemsManager& items, const AchievementsManager& achievements, const Sound& sound, GameState currentState, int brightness) {
    if (!initialized) begin();

    // 1. Fundo Dinâmico com base no Horário do Dia
    uint16_t bgCol = clock.getSkyColor();
    canvas.fillScreen(bgCol);

    // Chão / Quarto do Tamagotchi
    canvas.fillRect(0, 95, SCREEN_WIDTH, 40, 0x39E7); // Chão verde/marrom claro
    canvas.drawFastHLine(0, 95, SCREEN_WIDTH, 0x18C3);

    // Decorações de ambiente baseadas no horário real
    DayPhase phase = clock.getDayPhase();
    if (phase == PHASE_NIGHT) {
        // Lua Noturna
        canvas.fillCircle(210, 30, 8, TFT_YELLOW);
        canvas.fillCircle(207, 28, 7, bgCol); // Recorte meia-lua
        // Estrelas piscantes
        canvas.drawPixel(20, 25, TFT_WHITE);
        canvas.drawPixel(80, 18, TFT_YELLOW);
        canvas.drawPixel(140, 30, TFT_WHITE);
        canvas.drawPixel(170, 22, TFT_YELLOW);
        canvas.drawPixel(50, 45, TFT_WHITE);
    } else if (phase == PHASE_DUSK) {
        // Pôr do Sol (Céu alaranjado/rosado)
        canvas.fillCircle(200, 65, 10, TFT_ORANGE);
        canvas.fillCircle(40, 30, 8, COLOR_BG_DAWN);
        canvas.fillCircle(48, 28, 10, COLOR_BG_DAWN);
        canvas.fillCircle(56, 30, 8, COLOR_BG_DAWN);
    } else if (phase == PHASE_AFTERNOON) {
        // Tarde radiante
        canvas.fillCircle(205, 28, 10, TFT_YELLOW);
        canvas.fillCircle(40, 25, 8, TFT_WHITE);
        canvas.fillCircle(48, 23, 10, TFT_WHITE);
        canvas.fillCircle(56, 25, 8, TFT_WHITE);
    } else {
        // Manhã suave
        canvas.fillCircle(205, 30, 9, TFT_YELLOW);
        canvas.fillCircle(40, 25, 8, TFT_WHITE);
        canvas.fillCircle(48, 23, 10, TFT_WHITE);
        canvas.fillCircle(56, 25, 8, TFT_WHITE);
    }

    // 2. Cabeçalho Superior
    canvas.fillRect(0, 0, SCREEN_WIDTH, 18, COLOR_HEADER_BG);
    canvas.drawFastHLine(0, 18, SCREEN_WIDTH, COLOR_ACCENT);

    // Leitura da Bateria
    int batLevel = M5.Power.getBatteryLevel();
    if (batLevel < 0) batLevel = 100;

    // Nome, Sexo e Bateria no Canto Esquerdo
    char nameBatBuf[32];
    snprintf(nameBatBuf, sizeof(nameBatBuf), "%s(%s) | B:%d%%", pet.name, pet.getGenderSymbol(), batLevel);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(TL_DATUM);
    canvas.drawString(nameBatBuf, 4, 2);

    // Relógio Interno, Dia e Moedas no Canto Direito
    char timeBuf[16];
    clock.getFormattedTime(timeBuf, sizeof(timeBuf));
    char headerRight[48];
    snprintf(headerRight, sizeof(headerRight), "D%d | %s%s | %dC", pet.ageDays + 1, timeBuf, clock.getDevMode() ? "[D]" : "", pet.coins);
    canvas.setTextDatum(TR_DATUM);
    canvas.drawString(headerRight, SCREEN_WIDTH - 4, 2);

    // 3. Renderizar Cocôs no Chão
    for (int p = 0; p < pet.poopCount; p++) {
        int px = 25 + (p * 24);
        int py = 90;
        anim.drawPoop(canvas, px, py);
    }

    // 4. Renderizar Sprite do Pet
    int petX = SCREEN_WIDTH / 2;
    int petY = 75;
    anim.drawPetSprite(canvas, petX, petY, pet.stage, pet.variant, pet.getStatusText(), pet.isSleeping, pet.isSick, pet.isDead, pet.happiness);

    // Desenhar sujeiras se precisar de banho
    if (pet.isDirty && !pet.isDead) {
        canvas.fillCircle(petX - 18, petY - 5, 2, 0x5280);
        canvas.fillCircle(petX + 16, petY + 4, 3, 0x5280);
        canvas.fillCircle(petX - 10, petY + 12, 2, 0x5280);
    }

    // 5. Partículas
    anim.drawParticles(canvas);

    // 6. Notificações Popups
    events.drawNotification(canvas);

    // 7. Menu de Ações e Sub-menus
    if (currentState == STATE_MENU || menu.getActiveSubMenu() != SUBMENU_NONE) {
        menu.draw(canvas, pet, items, achievements, sound, clock, anim, brightness, currentState);
    } else {
        // Dica no Gameplay livre
        canvas.setTextDatum(BC_DATUM);
        canvas.setTextColor(TFT_LIGHTGRAY);
        canvas.drawString("[ENTER] Menu | [1] Carinho | [2] Limpar", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 2);
    }

    canvas.pushSprite(0, 0);
}

void Renderer::renderTitleScreen(Animation& anim) {
    if (!initialized) begin();

    canvas.fillScreen(COLOR_BG_NIGHT);

    // Título Logotipo Tamagotchi
    canvas.setTextColor(COLOR_COIN);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString("CARDPET TAMAGOTCHI", SCREEN_WIDTH / 2, 25);

    canvas.setTextColor(TFT_WHITE);
    canvas.drawString("M5Stack Cardputer Edition", SCREEN_WIDTH / 2, 45);

    // Pet no Centro
    anim.drawPetSprite(canvas, SCREEN_WIDTH / 2, 80, STAGE_BABY, PET_VARIANT_CLASSIC, "Happy", false, false, false, 90);

    // Prompt
    canvas.setTextColor(TFT_YELLOW);
    canvas.drawString("Pressione [ENTER] para Iniciar", SCREEN_WIDTH / 2, 118);

    canvas.pushSprite(0, 0);
}

void Renderer::renderGenderEntryScreen(PetGender currentSelection, Animation& anim) {
    if (!initialized) begin();

    canvas.fillScreen(COLOR_PANEL_BG);

    // Cabeçalho
    canvas.fillRect(0, 0, SCREEN_WIDTH, 20, COLOR_HEADER_BG);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString("ESCOLHA O SEXO DO PET", SCREEN_WIDTH / 2, 10);

    // Duas Caixas de Opções: Macho vs Fêmea
    int boxW = 90;
    int boxH = 65;

    // Caixa Macho (Esquerda)
    int mX = 22;
    int mY = 32;
    if (currentSelection == GENDER_MALE) {
        canvas.fillRoundRect(mX, mY, boxW, boxH, 6, 0x1A1F);
        canvas.drawRoundRect(mX, mY, boxW, boxH, 6, TFT_CYAN);
        canvas.drawRoundRect(mX + 1, mY + 1, boxW - 2, boxH - 2, 5, TFT_WHITE);
        canvas.setTextColor(TFT_YELLOW);
    } else {
        canvas.fillRoundRect(mX, mY, boxW, boxH, 6, 0x10A5);
        canvas.drawRoundRect(mX, mY, boxW, boxH, 6, TFT_DARKGRAY);
        canvas.setTextColor(TFT_WHITE);
    }
    canvas.drawString("Macho", mX + (boxW / 2), mY + 20);
    canvas.drawString("( M )", mX + (boxW / 2), mY + 44);

    // Caixa Fêmea (Direita)
    int fX = 128;
    int fY = 32;
    if (currentSelection == GENDER_FEMALE) {
        canvas.fillRoundRect(fX, fY, boxW, boxH, 6, 0x480F);
        canvas.drawRoundRect(fX, fY, boxW, boxH, 6, 0xF81F);
        canvas.drawRoundRect(fX + 1, fY + 1, boxW - 2, boxH - 2, 5, TFT_WHITE);
        canvas.setTextColor(TFT_YELLOW);
    } else {
        canvas.fillRoundRect(fX, fY, boxW, boxH, 6, 0x2005);
        canvas.drawRoundRect(fX, fY, boxW, boxH, 6, TFT_DARKGRAY);
        canvas.setTextColor(TFT_WHITE);
    }
    canvas.drawString("Femea", fX + (boxW / 2), fY + 20);
    canvas.drawString("( F )", fX + (boxW / 2), fY + 44);

    // Instruções no Rodapé
    canvas.setTextColor(TFT_LIGHTGRAY);
    canvas.drawString("[SETAS] Escolher sexo", SCREEN_WIDTH / 2, 106);
    canvas.setTextColor(TFT_YELLOW);
    canvas.drawString("[ENTER] Confirmar", SCREEN_WIDTH / 2, 120);

    canvas.pushSprite(0, 0);
}

void Renderer::renderNameEntryScreen(const char* inputName, PetGender gender, Animation& anim) {
    if (!initialized) begin();

    canvas.fillScreen(COLOR_PANEL_BG);

    // Cabeçalho
    canvas.fillRect(0, 0, SCREEN_WIDTH, 20, COLOR_HEADER_BG);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(MC_DATUM);
    char headBuf[32];
    snprintf(headBuf, sizeof(headBuf), "NOME DO PET (%s)", (gender == GENDER_FEMALE) ? "FEMEA" : "MACHO");
    canvas.drawString(headBuf, SCREEN_WIDTH / 2, 10);

    // Ovo no topo
    anim.drawPetSprite(canvas, SCREEN_WIDTH / 2, 42, STAGE_EGG, PET_VARIANT_CLASSIC, "Egg", false, false, false, 100);

    // Caixa de entrada de texto
    int boxX = 30;
    int boxY = 68;
    int boxW = 180;
    int boxH = 24;

    canvas.fillRoundRect(boxX, boxY, boxW, boxH, 4, TFT_BLACK);
    canvas.drawRoundRect(boxX, boxY, boxW, boxH, 4, COLOR_ACCENT);

    // Renderizar Texto Digitado com cursor piscante
    char displayBuf[32];
    snprintf(displayBuf, sizeof(displayBuf), "%s_", inputName);
    canvas.setTextColor(COLOR_COIN);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString(displayBuf, SCREEN_WIDTH / 2, boxY + 12);

    // Dicas de Teclado
    canvas.setTextColor(TFT_LIGHTGRAY);
    canvas.drawString("Digite no teclado (3 a 12 letras)", SCREEN_WIDTH / 2, 102);

    canvas.setTextColor(TFT_YELLOW);
    canvas.drawString("[ENTER] Confirmar | [DEL] Apagar", SCREEN_WIDTH / 2, 118);

    canvas.pushSprite(0, 0);
}

void Renderer::renderHatchingScreen(float progress, Animation& anim) {
    if (!initialized) begin();

    canvas.fillScreen(COLOR_BG_DAWN);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString("NASCIMENTO DO PET!", SCREEN_WIDTH / 2, 20);

    // Ovo Balançando
    int shakeX = (int)(sin(progress * 25.0f) * 4.0f);
    anim.drawPetSprite(canvas, (SCREEN_WIDTH / 2) + shakeX, 70, STAGE_EGG, PET_VARIANT_CLASSIC, "Hatching", false, false, false, 100);

    if (progress > 0.5f) {
        anim.spawnParticle((SCREEN_WIDTH / 2) + shakeX, 60, PARTICLE_STAR, COLOR_COIN);
    }

    canvas.setTextColor(COLOR_COIN);
    canvas.drawString("O ovo esta chocando...", SCREEN_WIDTH / 2, 108);

    canvas.pushSprite(0, 0);
}

void Renderer::renderEvolutionScreen(const Pet& pet, Animation& anim) {
    if (!initialized) begin();

    canvas.fillScreen(COLOR_BG_DAWN);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString("PARABENS!", SCREEN_WIDTH / 2, 20);
    canvas.drawString("Seu Tamagotchi Evoluiu!", SCREEN_WIDTH / 2, 45);

    anim.drawPetSprite(canvas, SCREEN_WIDTH / 2, 80, pet.stage, pet.variant, "Happy", false, false, false, 100);

    char buf[32];
    snprintf(buf, sizeof(buf), "Novo Estagio: %s", pet.getStageName());
    canvas.setTextColor(COLOR_COIN);
    canvas.drawString(buf, SCREEN_WIDTH / 2, 115);

    canvas.pushSprite(0, 0);
}

void Renderer::renderGameOverScreen(const Pet& pet, Animation& anim) {
    if (!initialized) begin();

    canvas.fillScreen(TFT_BLACK);
    canvas.setTextColor(TFT_RED);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString("GAME OVER", SCREEN_WIDTH / 2, 25);

    anim.drawPetSprite(canvas, SCREEN_WIDTH / 2, 65, pet.stage, pet.variant, "Dead", false, false, true, 0);

    canvas.setTextColor(TFT_WHITE);
    char buf[48];
    snprintf(buf, sizeof(buf), "%s viveu por %d dias", pet.name, pet.ageDays);
    canvas.drawString(buf, SCREEN_WIDTH / 2, 98);
    canvas.drawString("Pressione [ENTER] para Reiniciar", SCREEN_WIDTH / 2, 118);

    canvas.pushSprite(0, 0);
}

