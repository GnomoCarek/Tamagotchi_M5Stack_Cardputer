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

void Renderer::renderGameplay(const Pet& pet, const Clock& clock, Animation& anim, EventsManager& events, MenuManager& menu, const ItemsManager& items, const AchievementsManager& achievements, const Sound& sound, GameState currentState, float timeSpeed, int brightness) {
    if (!initialized) begin();

    // 1. Fundo Dinâmico com base no Horário do Dia
    uint16_t bgCol = clock.getSkyColor();
    canvas.fillScreen(bgCol);

    // Chão / Quarto do Tamagotchi
    canvas.fillRect(0, 95, SCREEN_WIDTH, 40, 0x39E7); // Chão verde/marrom claro
    canvas.drawFastHLine(0, 95, SCREEN_WIDTH, 0x18C3);

    // Decorações de ambiente (Sol/Nuvens de dia, Pôr do Sol no Crepúsculo, Lua/Estrelas à noite)
    DayPhase phase = clock.getDayPhase();
    if (phase == PHASE_NIGHT) {
        // Lua
        canvas.fillCircle(210, 30, 8, TFT_YELLOW);
        canvas.fillCircle(207, 28, 7, bgCol); // Recorte para formato de meia-lua
        // Estrelas piscantes
        canvas.drawPixel(20, 25, TFT_WHITE);
        canvas.drawPixel(80, 18, TFT_YELLOW);
        canvas.drawPixel(140, 30, TFT_WHITE);
        canvas.drawPixel(170, 22, TFT_YELLOW);
        canvas.drawPixel(50, 45, TFT_WHITE);
    } else if (phase == PHASE_DAWN || phase == PHASE_DUSK) {
        // Sol se pondo / nascendo
        canvas.fillCircle(200, 65, 10, TFT_ORANGE);
        // Nuvens rosadas/alaranjadas
        canvas.fillCircle(40, 30, 8, COLOR_BG_DAWN);
        canvas.fillCircle(48, 28, 10, COLOR_BG_DAWN);
        canvas.fillCircle(56, 30, 8, COLOR_BG_DAWN);
    } else {
        // Sol animado
        canvas.fillCircle(205, 30, 9, TFT_YELLOW);
        // Nuvens brancas
        canvas.fillCircle(40, 25, 8, TFT_WHITE);
        canvas.fillCircle(48, 23, 10, TFT_WHITE);
        canvas.fillCircle(56, 25, 8, TFT_WHITE);

        canvas.fillCircle(130, 35, 6, TFT_WHITE);
        canvas.fillCircle(136, 33, 8, TFT_WHITE);
        canvas.fillCircle(142, 35, 6, TFT_WHITE);
    }

    // 2. Cabeçalho Superior
    canvas.fillRect(0, 0, SCREEN_WIDTH, 18, COLOR_HEADER_BG);
    canvas.drawFastHLine(0, 18, SCREEN_WIDTH, COLOR_ACCENT);

    // Leitura da Bateria
    int batLevel = M5.Power.getBatteryLevel();
    if (batLevel < 0) batLevel = 100;

    // Nome e Bateria no Canto Esquerdo
    char nameBatBuf[32];
    snprintf(nameBatBuf, sizeof(nameBatBuf), "%s | B:%d%%", pet.name, batLevel);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(TL_DATUM);
    canvas.drawString(nameBatBuf, 4, 2);

    // Relógio Interno, Dia e Moedas no Canto Direito
    char timeBuf[16];
    clock.getFormattedTime(timeBuf, sizeof(timeBuf));
    char headerRight[48];
    snprintf(headerRight, sizeof(headerRight), "D%d | %s | %dC", pet.ageDays + 1, timeBuf, pet.coins);
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
        menu.draw(canvas, pet, items, achievements, sound, clock, anim, timeSpeed, brightness);
    } else {
        // Mini Dica no Gameplay livre
        canvas.setTextDatum(BC_DATUM);
        canvas.setTextColor(TFT_LIGHTGRAY);
        canvas.drawString("[ENTER] Menu | [ESPACO] Carinho", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 2);
    }

    // Push Sprite sem cintilação
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

void Renderer::renderNameEntryScreen(const char* inputName, Animation& anim) {
    if (!initialized) begin();

    canvas.fillScreen(COLOR_PANEL_BG);

    // Cabeçalho
    canvas.fillRect(0, 0, SCREEN_WIDTH, 20, COLOR_HEADER_BG);
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(MC_DATUM);
    canvas.drawString("ESCOLHA O NOME DO PET", SCREEN_WIDTH / 2, 10);

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

