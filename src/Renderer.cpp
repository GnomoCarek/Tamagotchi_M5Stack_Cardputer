#include "Renderer.h"

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

    // Decorações de ambiente (Nuvem de dia / Estrelas à noite)
    if (clock.getDayPhase() == PHASE_NIGHT) {
        canvas.drawPixel(20, 25, TFT_WHITE);
        canvas.drawPixel(80, 18, TFT_YELLOW);
        canvas.drawPixel(140, 30, TFT_WHITE);
        canvas.drawPixel(200, 22, TFT_YELLOW);
    } else {
        // Nuvens
        canvas.fillCircle(40, 25, 8, TFT_WHITE);
        canvas.fillCircle(48, 23, 10, TFT_WHITE);
        canvas.fillCircle(56, 25, 8, TFT_WHITE);
    }

    // 2. Cabeçalho Superior
    canvas.fillRect(0, 0, SCREEN_WIDTH, 18, COLOR_HEADER_BG);
    canvas.drawFastHLine(0, 18, SCREEN_WIDTH, COLOR_ACCENT);

    // Nome e Ícone do Pet
    canvas.setTextColor(TFT_WHITE);
    canvas.setTextDatum(TL_DATUM);
    canvas.drawString(pet.name, 6, 2);

    // Relógio Interno & Dia
    char timeBuf[32];
    clock.getFormattedTime(timeBuf, sizeof(timeBuf));
    char headerRight[48];
    snprintf(headerRight, sizeof(headerRight), "Dia %d | %s | %dC", pet.ageDays + 1, timeBuf, pet.coins);
    canvas.setTextDatum(TR_DATUM);
    canvas.drawString(headerRight, SCREEN_WIDTH - 6, 2);

    // 3. Renderizar Cocôs no Chão
    for (int p = 0; p < pet.poopCount; p++) {
        int px = 30 + (p * 25);
        int py = 90;
        anim.drawPoop(canvas, px, py);
    }

    // 4. Renderizar Sprite do Pet
    int petX = SCREEN_WIDTH / 2;
    int petY = 75;
    anim.drawPetSprite(canvas, petX, petY, pet.stage, pet.variant, pet.getStatusText(), pet.isSleeping, pet.isSick, pet.isDead, pet.happiness);

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
    canvas.drawString("TAMAGOTCHI", SCREEN_WIDTH / 2, 30);

    canvas.setTextColor(TFT_WHITE);
    canvas.drawString("M5Stack Cardputer Edition", SCREEN_WIDTH / 2, 55);

    // Pet no Centro
    anim.drawPetSprite(canvas, SCREEN_WIDTH / 2, 85, STAGE_BABY, PET_VARIANT_CLASSIC, "Happy", false, false, false, 90);

    // Prompt
    canvas.setTextColor(TFT_YELLOW);
    canvas.drawString("Pressione [ENTER] para Iniciar", SCREEN_WIDTH / 2, 120);

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
