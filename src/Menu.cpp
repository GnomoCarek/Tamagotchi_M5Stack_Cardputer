#include "Menu.h"

#define MAIN_MENU_COUNT 13

static const char* mainMenuItems[] = {
    "Alimentar", "Brincar", "Remedio", "Dormir",
    "Banho", "Limpar", "Carinho", "Status",
    "Loja", "Inventario", "Conquistas", "Salvar", "Config"
};

MenuManager::MenuManager() {
    mainIndex = 0;
    subIndex = 0;
    activeSubMenu = SUBMENU_NONE;
    settingsOption = 0;
    confirmReset = false;
}

void MenuManager::update(KeyAction action, Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, float& timeSpeed, int& brightness, bool& requestSave, bool& requestReset, GameState& currentState) {
    if (activeSubMenu == SUBMENU_NONE) {
        // Navegação do Menu Principal
        if (action == ACTION_LEFT) {
            mainIndex = (mainIndex - 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_RIGHT) {
            mainIndex = (mainIndex + 1) % MAIN_MENU_COUNT;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_SELECT) {
            sound.playSound(SOUND_CLICK);
            switch (mainIndex) {
                case 0: // Alimentar
                    activeSubMenu = SUBMENU_INVENTORY;
                    subIndex = 0;
                    break;
                case 1: // Brincar -> Inicia Minijogo
                    currentState = STATE_MINIGAME;
                    break;
                case 2: // Remédio
                    if (pet.isSick) {
                        if (pet.cure()) sound.playSound(SOUND_PLAY);
                    } else {
                        activeSubMenu = SUBMENU_INVENTORY;
                    }
                    break;
                case 3: // Dormir
                    pet.toggleSleep();
                    sound.playSound(SOUND_SLEEP);
                    break;
                case 4: // Banho
                    if (pet.giveBath()) sound.playSound(SOUND_CLEAN);
                    break;
                case 5: // Limpar Cocô
                    if (pet.cleanPoop()) sound.playSound(SOUND_CLEAN);
                    break;
                case 6: // Carinho
                    if (pet.petCare()) sound.playSound(SOUND_PLAY);
                    break;
                case 7: // Status
                    activeSubMenu = SUBMENU_STATUS;
                    break;
                case 8: // Loja
                    activeSubMenu = SUBMENU_SHOP;
                    subIndex = 0;
                    break;
                case 9: // Inventário
                    activeSubMenu = SUBMENU_INVENTORY;
                    subIndex = 0;
                    break;
                case 10: // Conquistas
                    activeSubMenu = SUBMENU_ACHIEVEMENTS;
                    subIndex = 0;
                    break;
                case 11: // Salvar
                    requestSave = true;
                    break;
                case 12: // Configurações
                    activeSubMenu = SUBMENU_SETTINGS;
                    settingsOption = 0;
                    break;
            }
        } else if (action == ACTION_BACK) {
            currentState = STATE_GAMEPLAY;
        }
    } else {
        // Navegação de Sub-menus
        if (action == ACTION_BACK) {
            sound.playSound(SOUND_CLICK);
            if (confirmReset) {
                confirmReset = false;
            } else {
                activeSubMenu = SUBMENU_NONE;
            }
            return;
        }

        if (activeSubMenu == SUBMENU_SHOP) {
            if (action == ACTION_UP) {
                subIndex = (subIndex - 1 + items.getCatalogSize()) % items.getCatalogSize();
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_DOWN) {
                subIndex = (subIndex + 1) % items.getCatalogSize();
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_SELECT) {
                if (items.buyItem(subIndex, pet.coins)) {
                    sound.playSound(SOUND_COIN);
                } else {
                    sound.playSound(SOUND_SICK);
                }
            }
        } else if (activeSubMenu == SUBMENU_INVENTORY) {
            if (action == ACTION_UP) {
                subIndex = (subIndex - 1 + items.getCatalogSize()) % items.getCatalogSize();
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_DOWN) {
                subIndex = (subIndex + 1) % items.getCatalogSize();
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_SELECT) {
                if (items.useItem(subIndex, pet)) {
                    sound.playSound(SOUND_FEED);
                } else {
                    sound.playSound(SOUND_SICK);
                }
            }
        } else if (activeSubMenu == SUBMENU_SETTINGS) {
            if (action == ACTION_UP) {
                settingsOption = (settingsOption - 1 + 6) % 6;
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_DOWN) {
                settingsOption = (settingsOption + 1) % 6;
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_LEFT || action == ACTION_RIGHT) {
                int dir = (action == ACTION_RIGHT) ? 1 : -1;
                sound.playSound(SOUND_CLICK);
                switch (settingsOption) {
                    case 0: sound.setVolume(sound.getVolume() + dir); break;
                    case 1: brightness = constrain(brightness + (dir * 25), 25, 255); M5Cardputer.Display.setBrightness(brightness); break;
                    case 2:
                        if (pet.screenTimeoutSec == 0) pet.screenTimeoutSec = (dir > 0) ? 30 : 120;
                        else if (pet.screenTimeoutSec == 30) pet.screenTimeoutSec = (dir > 0) ? 60 : 0;
                        else if (pet.screenTimeoutSec == 60) pet.screenTimeoutSec = (dir > 0) ? 120 : 30;
                        else if (pet.screenTimeoutSec == 120) pet.screenTimeoutSec = (dir > 0) ? 0 : 60;
                        break;
                    case 3: sound.setSoundEnabled(!sound.isSoundEnabled()); break;
                    case 4: sound.setBgmEnabled(!sound.isBgmEnabled()); break;
                    case 5: confirmReset = true; break;
                }
            } else if (action == ACTION_SELECT) {
                if (settingsOption == 5) {
                    if (confirmReset) {
                        requestReset = true;
                        activeSubMenu = SUBMENU_NONE;
                    } else {
                        confirmReset = true;
                    }
                }
            }
        } else if (activeSubMenu == SUBMENU_ACHIEVEMENTS || activeSubMenu == SUBMENU_STATUS) {
            if (action == ACTION_UP || action == ACTION_DOWN) {
                subIndex = (subIndex + 1) % 10;
                sound.playSound(SOUND_CLICK);
            }
        }
    }
}

void MenuManager::draw(M5Canvas& canvas, const Pet& pet, const ItemsManager& items, const AchievementsManager& achievements, const Sound& sound, const Clock& clock, Animation& anim, float timeSpeed, int brightness) {
    if (activeSubMenu == SUBMENU_NONE) {
        // Banner flutuante com o nome da opção selecionada (para não sobrepor os ícones)
        int bannerW = 120;
        int bannerH = 18;
        int bannerX = (SCREEN_WIDTH - bannerW) / 2;
        int bannerY = SCREEN_HEIGHT - 46;

        canvas.fillRoundRect(bannerX, bannerY, bannerW, bannerH, 4, COLOR_PANEL_BG);
        canvas.drawRoundRect(bannerX, bannerY, bannerW, bannerH, 4, COLOR_ACCENT);
        canvas.setTextColor(TFT_YELLOW);
        canvas.setTextDatum(MC_DATUM);
        canvas.drawString(mainMenuItems[mainIndex], SCREEN_WIDTH / 2, bannerY + 9);

        // Barra do Menu Principal no Rodapé (Apenas Ícones)
        int barH = 24;
        int barY = SCREEN_HEIGHT - barH;
        canvas.fillRect(0, barY, SCREEN_WIDTH, barH, COLOR_PANEL_BG);
        canvas.drawFastHLine(0, barY, SCREEN_WIDTH, COLOR_ACCENT);

        // Desenhar 5 ícones centralizados
        int totalVisible = 5;
        int itemW = 42;
        int startX = 16;

        int firstIdx = max(0, min(mainIndex - 2, MAIN_MENU_COUNT - totalVisible));

        for (int i = 0; i < totalVisible; i++) {
            int idx = firstIdx + i;
            if (idx >= MAIN_MENU_COUNT) break;

            int ix = startX + (i * itemW);
            int iy = barY + 4;

            if (idx == mainIndex) {
                canvas.fillRoundRect(ix - 3, iy - 2, 20, 18, 3, COLOR_ACCENT);
                canvas.drawRoundRect(ix - 3, iy - 2, 20, 18, 3, TFT_YELLOW);
            }

            anim.drawIcon(canvas, ix, iy, idx, (idx == mainIndex) ? TFT_YELLOW : TFT_LIGHTGRAY);
        }
    } else {
        // Renderização dos Sub-menus
        canvas.fillScreen(COLOR_PANEL_BG);

        // Cabeçalho do Submenu
        canvas.fillRect(0, 0, SCREEN_WIDTH, 20, COLOR_HEADER_BG);
        canvas.setTextColor(TFT_WHITE);
        canvas.setTextDatum(MC_DATUM);

        if (activeSubMenu == SUBMENU_STATUS) {
            canvas.drawString("STATUS DO PET", SCREEN_WIDTH / 2, 10);

            canvas.setTextDatum(TL_DATUM);
            char buf[64];
            snprintf(buf, sizeof(buf), "%s | %s | %dd", pet.name, pet.getStageName(), pet.ageDays);
            canvas.drawString(buf, 10, 24);
            snprintf(buf, sizeof(buf), "Perfil: %s | Peso: %dkg", pet.getPersonalityName(), pet.weight);
            canvas.drawString(buf, 10, 36);

            // Barras de Status
            int barX = 75;
            int barW = 145;

            canvas.drawString("Saude:", 10, 50);
            canvas.fillRect(barX, 50, barW, 7, COLOR_BAR_BG);
            canvas.fillRect(barX, 50, (pet.health * barW) / 100, 7, COLOR_HEALTH);

            canvas.drawString("Fome:", 10, 62);
            canvas.fillRect(barX, 62, barW, 7, COLOR_BAR_BG);
            canvas.fillRect(barX, 62, (pet.hunger * barW) / 100, 7, COLOR_HUNGER);

            canvas.drawString("Feliz:", 10, 74);
            canvas.fillRect(barX, 74, barW, 7, COLOR_BAR_BG);
            canvas.fillRect(barX, 74, (pet.happiness * barW) / 100, 7, COLOR_HAPPY);

            canvas.drawString("Energia:", 10, 86);
            canvas.fillRect(barX, 86, barW, 7, COLOR_BAR_BG);
            canvas.fillRect(barX, 86, (pet.energy * barW) / 100, 7, COLOR_ENERGY);

            canvas.drawString("Carinho:", 10, 98);
            canvas.fillRect(barX, 98, barW, 7, COLOR_BAR_BG);
            canvas.fillRect(barX, 98, (pet.affection * barW) / 100, 7, 0xF81F);

            canvas.drawString(pet.isDirty ? "Banho: Sujo!" : "Banho: Limpo", 10, 110);
            canvas.drawString(pet.isSick ? "Saude: Doente!" : "Saude: Ok", 120, 110);

            canvas.setTextDatum(BC_DATUM);
            canvas.drawString("[ESC/Q] Voltar", SCREEN_WIDTH / 2, 133);

        } else if (activeSubMenu == SUBMENU_SHOP || activeSubMenu == SUBMENU_INVENTORY) {
            bool isShop = (activeSubMenu == SUBMENU_SHOP);
            canvas.drawString(isShop ? "LOJA" : "INVENTARIO", SCREEN_WIDTH / 2, 10);

            int startItem = max(0, min(subIndex - 1, items.getCatalogSize() - 3));
            for (int i = 0; i < 3; i++) {
                int idx = startItem + i;
                if (idx >= items.getCatalogSize()) break;

                const Item& item = items.getItem(idx);
                int iy = 24 + (i * 32);

                if (idx == subIndex) {
                    canvas.fillRoundRect(6, iy, SCREEN_WIDTH - 12, 30, 4, COLOR_ACCENT);
                    canvas.drawRoundRect(6, iy, SCREEN_WIDTH - 12, 30, 4, TFT_YELLOW);
                } else {
                    canvas.fillRoundRect(6, iy, SCREEN_WIDTH - 12, 30, 4, 0x2104);
                }

                anim.drawFood(canvas, 12, iy + 7, item.id);

                canvas.setTextDatum(TL_DATUM);
                canvas.setTextColor(TFT_WHITE);
                canvas.drawString(item.name, 36, iy + 4);

                canvas.setTextColor(TFT_LIGHTGRAY);
                canvas.drawString(item.description, 36, iy + 17);

                canvas.setTextDatum(TR_DATUM);
                if (isShop) {
                    canvas.setTextColor(COLOR_COIN);
                    char pBuf[16];
                    snprintf(pBuf, sizeof(pBuf), "%d Moedas", item.price);
                    canvas.drawString(pBuf, SCREEN_WIDTH - 12, iy + 8);
                } else {
                    canvas.setTextColor(TFT_GREEN);
                    char qBuf[16];
                    snprintf(qBuf, sizeof(qBuf), "Qtd: %d", items.getQuantity(idx));
                    canvas.drawString(qBuf, SCREEN_WIDTH - 12, iy + 8);
                }
            }

            canvas.setTextDatum(BC_DATUM);
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString(isShop ? "[ENTER] Comprar | [ESC] Sair" : "[ENTER] Usar | [ESC] Sair", SCREEN_WIDTH / 2, 133);

        } else if (activeSubMenu == SUBMENU_SETTINGS) {
            canvas.drawString("CONFIGURACOES", SCREEN_WIDTH / 2, 10);

            const char* optNames[] = {"Volume", "Brilho Tela", "Timeout Tela", "Sons EFX", "Musica BGM", "Resetar Save"};
            for (int i = 0; i < 6; i++) {
                int iy = 24 + (i * 16);
                if (i == settingsOption) {
                    canvas.fillRect(10, iy - 1, SCREEN_WIDTH - 20, 15, COLOR_ACCENT);
                    canvas.setTextColor(TFT_YELLOW);
                } else {
                    canvas.setTextColor(TFT_WHITE);
                }

                canvas.setTextDatum(TL_DATUM);
                canvas.drawString(optNames[i], 16, iy);

                canvas.setTextDatum(TR_DATUM);
                char valBuf[32];
                if (i == 0) snprintf(valBuf, sizeof(valBuf), "< %d >", sound.getVolume());
                else if (i == 1) snprintf(valBuf, sizeof(valBuf), "< %d >", brightness);
                else if (i == 2) {
                    if (pet.screenTimeoutSec == 0) snprintf(valBuf, sizeof(valBuf), "< Nunca >");
                    else snprintf(valBuf, sizeof(valBuf), "< %ds >", pet.screenTimeoutSec);
                }
                else if (i == 3) snprintf(valBuf, sizeof(valBuf), "< %s >", sound.isSoundEnabled() ? "LIGADO" : "DESL");
                else if (i == 4) snprintf(valBuf, sizeof(valBuf), "< %s >", sound.isBgmEnabled() ? "LIGADO" : "DESL");
                else if (i == 5) snprintf(valBuf, sizeof(valBuf), "%s", confirmReset ? "CONFIRMAR?" : "[ SELECIONAR ]");

                canvas.drawString(valBuf, SCREEN_WIDTH - 16, iy);
            }

            canvas.setTextDatum(BC_DATUM);
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString("[SETAS] Ajustar | [ESC] Sair", SCREEN_WIDTH / 2, 133);
        } else if (activeSubMenu == SUBMENU_ACHIEVEMENTS) {
            canvas.drawString("CONQUISTAS (TROFEUS)", SCREEN_WIDTH / 2, 10);
            for (int i = 0; i < 4; i++) {
                int idx = (subIndex + i) % achievements.getCount();
                const Achievement& ach = achievements.getAchievement(idx);
                int iy = 24 + (i * 24);

                canvas.setTextDatum(TL_DATUM);
                canvas.setTextColor(ach.unlocked ? COLOR_COIN : TFT_DARKGRAY);
                canvas.drawString(ach.unlocked ? "[V]" : "[X]", 12, iy);
                canvas.drawString(ach.title, 34, iy);

                canvas.setTextColor(TFT_LIGHTGRAY);
                canvas.drawString(ach.description, 34, iy + 10);
            }
            canvas.setTextDatum(BC_DATUM);
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString("[SETAS] Rolar | [ESC] Sair", SCREEN_WIDTH / 2, 133);
        }
    }
}

