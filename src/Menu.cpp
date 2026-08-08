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
    activeCategory = ITEM_FOOD;
    settingsOption = 0;
    confirmReset = false;

    wifiFieldStep = 0;
    wifiSsidBuf[0] = '\0';
    wifiPassBuf[0] = '\0';
    wifiSsidLen = 0;
    wifiPassLen = 0;
    wifiStatusMsg[0] = '\0';

    initialMenuIndex = 0;
    selectedGender = GENDER_MALE;
    minigameSelectIndex = 0;
}

void MenuManager::openSubMenu(SubMenuType sub, ItemType cat) {
    activeSubMenu = sub;
    activeCategory = cat;
    subIndex = 0;
}

void MenuManager::update(KeyAction action, char typedChar, Pet& pet, ItemsManager& items, AchievementsManager& achievements, Sound& sound, Clock& clock, int& brightness, bool& requestSave, bool& requestReset, GameState& currentState) {
    // -------------------------------------------------------------
    // ESTADO: MENU INICIAL (QUANDO EXISTE SAVE)
    // -------------------------------------------------------------
    if (currentState == STATE_INITIAL_MENU) {
        if (action == ACTION_UP) {
            initialMenuIndex = (initialMenuIndex - 1 + 3) % 3;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_DOWN) {
            initialMenuIndex = (initialMenuIndex + 1) % 3;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_SELECT) {
            sound.playSound(SOUND_CLICK);
            if (initialMenuIndex == 0) { // Continuar
                currentState = STATE_GAMEPLAY;
            } else if (initialMenuIndex == 1) { // Novo Jogo
                currentState = STATE_GENDER_ENTRY;
            } else if (initialMenuIndex == 2) { // Configurações
                activeSubMenu = SUBMENU_SETTINGS;
                settingsOption = 0;
                currentState = STATE_MENU;
            }
        }
        return;
    }

    // -------------------------------------------------------------
    // ESTADO: SELEÇÃO DE SEXO (NOVO JOGO)
    // -------------------------------------------------------------
    if (currentState == STATE_GENDER_ENTRY) {
        if (action == ACTION_LEFT || action == ACTION_RIGHT || action == ACTION_UP || action == ACTION_DOWN) {
            selectedGender = (selectedGender == GENDER_MALE) ? GENDER_FEMALE : GENDER_MALE;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_SELECT) {
            sound.playSound(SOUND_CLICK);
            currentState = STATE_NAME_ENTRY;
        }
        return;
    }

    // -------------------------------------------------------------
    // ESTADO: SELEÇÃO DE MINIJOGO
    // -------------------------------------------------------------
    if (currentState == STATE_MINIGAME_SELECT) {
        if (action == ACTION_UP || action == ACTION_LEFT) {
            minigameSelectIndex = 0;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_DOWN || action == ACTION_RIGHT) {
            minigameSelectIndex = 1;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_SELECT) {
            sound.playSound(SOUND_CLICK);
            if (minigameSelectIndex == 0) {
                currentState = STATE_MINIGAME_CATCH;
            } else {
                currentState = STATE_MINIGAME_REFLEX;
            }
        } else if (action == ACTION_BACK) {
            sound.playSound(SOUND_CLICK);
            currentState = STATE_GAMEPLAY;
        }
        return;
    }

    // -------------------------------------------------------------
    // GAMEPLAY PRINCIPAL / SUBMENUS
    // -------------------------------------------------------------
    if (activeSubMenu == SUBMENU_NONE) {
        // Navegação do Menu Principal (Barra de Ícones)
        if (action == ACTION_LEFT) {
            mainIndex = (mainIndex - 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_RIGHT) {
            mainIndex = (mainIndex + 1) % MAIN_MENU_COUNT;
            sound.playSound(SOUND_CLICK);
        } else if (action == ACTION_SELECT) {
            sound.playSound(SOUND_CLICK);
            switch (mainIndex) {
                case 0: // Alimentar -> Mostra Apenas Alimentos!
                    openSubMenu(SUBMENU_INVENTORY, ITEM_FOOD);
                    break;
                case 1: // Brincar -> Escolher Minijogo
                    currentState = STATE_MINIGAME_SELECT;
                    break;
                case 2: // Remédio
                    if (pet.isSick) {
                        if (pet.cure()) sound.playSound(SOUND_PLAY);
                    } else {
                        openSubMenu(SUBMENU_INVENTORY, ITEM_MEDICINE);
                    }
                    break;
                case 3: // Dormir
                    pet.toggleSleep();
                    sound.playSound(SOUND_SLEEP);
                    break;
                case 4: // Banho
                    if (pet.giveBath()) sound.playSound(SOUND_CLEAN);
                    else openSubMenu(SUBMENU_INVENTORY, ITEM_HYGIENE);
                    break;
                case 5: // Limpar Cocô
                    if (pet.cleanPoop()) sound.playSound(SOUND_CLEAN);
                    else openSubMenu(SUBMENU_INVENTORY, ITEM_CLEANING);
                    break;
                case 6: // Carinho
                    if (pet.petCare()) sound.playSound(SOUND_PLAY);
                    break;
                case 7: // Status
                    openSubMenu(SUBMENU_STATUS);
                    break;
                case 8: // Loja por Categorias
                    openSubMenu(SUBMENU_SHOP, ITEM_FOOD);
                    break;
                case 9: // Inventário por Categorias
                    openSubMenu(SUBMENU_INVENTORY, ITEM_FOOD);
                    break;
                case 10: // Conquistas
                    openSubMenu(SUBMENU_ACHIEVEMENTS);
                    break;
                case 11: // Salvar
                    requestSave = true;
                    break;
                case 12: // Configurações
                    openSubMenu(SUBMENU_SETTINGS);
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
            } else if (activeSubMenu == SUBMENU_WIFI) {
                activeSubMenu = SUBMENU_SETTINGS;
            } else {
                activeSubMenu = SUBMENU_NONE;
            }
            return;
        }

        if (activeSubMenu == SUBMENU_SHOP || activeSubMenu == SUBMENU_INVENTORY) {
            bool isShop = (activeSubMenu == SUBMENU_SHOP);
            int categoryItems = items.getCategoryItemCount(activeCategory);

            // Alternar Abas de Categoria com Setas Direita/Esquerda
            if (action == ACTION_LEFT) {
                activeCategory = (ItemType)((activeCategory - 1 + ITEM_CATEGORY_COUNT) % ITEM_CATEGORY_COUNT);
                subIndex = 0;
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_RIGHT) {
                activeCategory = (ItemType)((activeCategory + 1) % ITEM_CATEGORY_COUNT);
                subIndex = 0;
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_UP) {
                if (categoryItems > 0) {
                    subIndex = (subIndex - 1 + categoryItems) % categoryItems;
                    sound.playSound(SOUND_CLICK);
                }
            } else if (action == ACTION_DOWN) {
                if (categoryItems > 0) {
                    subIndex = (subIndex + 1) % categoryItems;
                    sound.playSound(SOUND_CLICK);
                }
            } else if (action == ACTION_SELECT) {
                if (categoryItems > 0) {
                    int itemId = items.getCategoryItemId(activeCategory, subIndex);
                    if (isShop) {
                        if (items.buyItem(itemId, pet.coins)) {
                            sound.playSound(SOUND_COIN);
                        } else {
                            sound.playSound(SOUND_SICK);
                        }
                    } else {
                        if (items.useItem(itemId, pet)) {
                            sound.playSound(SOUND_FEED);
                        } else {
                            sound.playSound(SOUND_SICK);
                        }
                    }
                }
            }
        } else if (activeSubMenu == SUBMENU_SETTINGS) {
            if (action == ACTION_UP) {
                settingsOption = (settingsOption - 1 + 8) % 8;
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_DOWN) {
                settingsOption = (settingsOption + 1) % 8;
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
                    case 5: clock.setDevMode(!clock.getDevMode()); break; // Modo Dev (30m = 1 dia)
                    case 6: // Configurar Wi-Fi
                        activeSubMenu = SUBMENU_WIFI;
                        wifiFieldStep = 0;
                        wifiStatusMsg[0] = '\0';
                        strncpy(wifiSsidBuf, clock.getWifiSsid(), sizeof(wifiSsidBuf) - 1);
                        wifiSsidLen = strlen(wifiSsidBuf);
                        strncpy(wifiPassBuf, clock.getWifiPass(), sizeof(wifiPassBuf) - 1);
                        wifiPassLen = strlen(wifiPassBuf);
                        break;
                    case 7: confirmReset = true; break;
                }
            } else if (action == ACTION_SELECT) {
                if (settingsOption == 6) { // Configurar Wi-Fi
                    activeSubMenu = SUBMENU_WIFI;
                    wifiFieldStep = 0;
                    wifiStatusMsg[0] = '\0';
                    strncpy(wifiSsidBuf, clock.getWifiSsid(), sizeof(wifiSsidBuf) - 1);
                    wifiSsidLen = strlen(wifiSsidBuf);
                    strncpy(wifiPassBuf, clock.getWifiPass(), sizeof(wifiPassBuf) - 1);
                    wifiPassLen = strlen(wifiPassBuf);
                } else if (settingsOption == 7) {
                    if (confirmReset) {
                        requestReset = true;
                        activeSubMenu = SUBMENU_NONE;
                    } else {
                        confirmReset = true;
                    }
                }
            }
        } else if (activeSubMenu == SUBMENU_WIFI) {
            // Digitação do Wi-Fi (SSID e Senha)
            if (action == ACTION_UP) {
                wifiFieldStep = 0;
                sound.playSound(SOUND_CLICK);
            } else if (action == ACTION_DOWN) {
                wifiFieldStep = 1;
                sound.playSound(SOUND_CLICK);
            }

            if (typedChar == '\b' || action == ACTION_DELETE) {
                if (wifiFieldStep == 0 && wifiSsidLen > 0) {
                    wifiSsidLen--;
                    wifiSsidBuf[wifiSsidLen] = '\0';
                    sound.playSound(SOUND_CLICK);
                } else if (wifiFieldStep == 1 && wifiPassLen > 0) {
                    wifiPassLen--;
                    wifiPassBuf[wifiPassLen] = '\0';
                    sound.playSound(SOUND_CLICK);
                }
            } else if (typedChar >= ' ' && typedChar <= '~') {
                if (wifiFieldStep == 0 && wifiSsidLen < 31) {
                    wifiSsidBuf[wifiSsidLen++] = typedChar;
                    wifiSsidBuf[wifiSsidLen] = '\0';
                    sound.playSound(SOUND_TYPING);
                } else if (wifiFieldStep == 1 && wifiPassLen < 63) {
                    wifiPassBuf[wifiPassLen++] = typedChar;
                    wifiPassBuf[wifiPassLen] = '\0';
                    sound.playSound(SOUND_TYPING);
                }
            }

            if (action == ACTION_SELECT) {
                if (wifiFieldStep == 0) {
                    wifiFieldStep = 1;
                    sound.playSound(SOUND_CLICK);
                } else {
                    // Tenta Conectar
                    clock.setWifiCredentials(wifiSsidBuf, wifiPassBuf);
                    snprintf(wifiStatusMsg, sizeof(wifiStatusMsg), "Conectando...");
                    if (clock.tryConnectWifi(6000)) {
                        snprintf(wifiStatusMsg, sizeof(wifiStatusMsg), "Conectado! NTP OK");
                        sound.playSound(SOUND_WIFI_OK);
                    } else {
                        snprintf(wifiStatusMsg, sizeof(wifiStatusMsg), "Falha na conexao!");
                        sound.playSound(SOUND_WRONG);
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

void MenuManager::draw(M5Canvas& canvas, const Pet& pet, const ItemsManager& items, const AchievementsManager& achievements, const Sound& sound, const Clock& clock, Animation& anim, int brightness, GameState currentState) {
    if (currentState == STATE_INITIAL_MENU) {
        // Renderizar Menu Inicial com Save
        canvas.fillScreen(COLOR_BG_NIGHT);
        canvas.setTextColor(COLOR_COIN);
        canvas.setTextDatum(MC_DATUM);
        canvas.drawString("CARDPET TAMAGOTCHI", SCREEN_WIDTH / 2, 20);

        const char* menuOpts[] = {"1. CONTINUAR", "2. NOVO JOGO", "3. CONFIGURACOES"};
        for (int i = 0; i < 3; i++) {
            int iy = 50 + (i * 24);
            if (i == initialMenuIndex) {
                canvas.fillRoundRect(30, iy - 4, 180, 20, 4, COLOR_ACCENT);
                canvas.drawRoundRect(30, iy - 4, 180, 20, 4, TFT_YELLOW);
                canvas.setTextColor(TFT_YELLOW);
            } else {
                canvas.fillRoundRect(30, iy - 4, 180, 20, 4, COLOR_PANEL_BG);
                canvas.setTextColor(TFT_WHITE);
            }
            canvas.drawString(menuOpts[i], SCREEN_WIDTH / 2, iy + 6);
        }

        canvas.setTextColor(TFT_LIGHTGRAY);
        canvas.drawString("[SETAS] Navegar | [ENTER] Confirmar", SCREEN_WIDTH / 2, 122);
        return;
    }

    if (currentState == STATE_MINIGAME_SELECT) {
        // Renderizar Seleção de Minijogos
        canvas.fillScreen(COLOR_PANEL_BG);
        canvas.fillRect(0, 0, SCREEN_WIDTH, 20, COLOR_HEADER_BG);
        canvas.setTextColor(TFT_WHITE);
        canvas.setTextDatum(MC_DATUM);
        canvas.drawString("ESCOLHA O MINIJOGO", SCREEN_WIDTH / 2, 10);

        const char* gOpts[] = {"1. Capturar Estrelas", "2. Teste de Reflexo"};
        const char* gDescs[] = {"Pegue estrelas e evite bombas", "Pressione as teclas rapidamente!"};

        for (int i = 0; i < 2; i++) {
            int iy = 34 + (i * 42);
            if (i == minigameSelectIndex) {
                canvas.fillRoundRect(15, iy, SCREEN_WIDTH - 30, 36, 5, COLOR_ACCENT);
                canvas.drawRoundRect(15, iy, SCREEN_WIDTH - 30, 36, 5, TFT_YELLOW);
                canvas.setTextColor(TFT_YELLOW);
            } else {
                canvas.fillRoundRect(15, iy, SCREEN_WIDTH - 30, 36, 5, 0x2104);
                canvas.setTextColor(TFT_WHITE);
            }
            canvas.setTextDatum(TL_DATUM);
            canvas.drawString(gOpts[i], 25, iy + 6);
            canvas.setTextColor(TFT_LIGHTGRAY);
            canvas.drawString(gDescs[i], 25, iy + 20);
        }

        canvas.setTextDatum(BC_DATUM);
        canvas.setTextColor(TFT_WHITE);
        canvas.drawString("[SETAS] Escolher | [ENTER] Jogar | [ESC] Sair", SCREEN_WIDTH / 2, 130);
        return;
    }

    if (activeSubMenu == SUBMENU_NONE) {
        // Banner flutuante com o nome da opção selecionada
        int bannerW = 120;
        int bannerH = 18;
        int bannerX = (SCREEN_WIDTH - bannerW) / 2;
        int bannerY = SCREEN_HEIGHT - 46;

        canvas.fillRoundRect(bannerX, bannerY, bannerW, bannerH, 4, COLOR_PANEL_BG);
        canvas.drawRoundRect(bannerX, bannerY, bannerW, bannerH, 4, COLOR_ACCENT);
        canvas.setTextColor(TFT_YELLOW);
        canvas.setTextDatum(MC_DATUM);
        canvas.drawString(mainMenuItems[mainIndex], SCREEN_WIDTH / 2, bannerY + 9);

        // Barra do Menu Principal no Rodapé
        int barH = 24;
        int barY = SCREEN_HEIGHT - barH;
        canvas.fillRect(0, barY, SCREEN_WIDTH, barH, COLOR_PANEL_BG);
        canvas.drawFastHLine(0, barY, SCREEN_WIDTH, COLOR_ACCENT);

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
        canvas.fillRect(0, 0, SCREEN_WIDTH, 20, COLOR_HEADER_BG);
        canvas.setTextColor(TFT_WHITE);
        canvas.setTextDatum(MC_DATUM);

        if (activeSubMenu == SUBMENU_STATUS) {
            canvas.drawString("STATUS DO PET", SCREEN_WIDTH / 2, 10);

            canvas.setTextDatum(TL_DATUM);
            char buf[64];
            snprintf(buf, sizeof(buf), "%s (%s) | %s | %dd", pet.name, pet.getGenderSymbol(), pet.getStageName(), pet.ageDays);
            canvas.drawString(buf, 10, 24);
            snprintf(buf, sizeof(buf), "Perfil: %s | Sexo: %s | %dkg", pet.getPersonalityName(), pet.getGenderName(), pet.weight);
            canvas.drawString(buf, 10, 36);

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

            // Barra de Categorias (Abas)
            int tabY = 22;
            canvas.setTextDatum(MC_DATUM);
            char catTabStr[64];
            snprintf(catTabStr, sizeof(catTabStr), "< %s > (%d/%d)", items.getCategoryName(activeCategory), activeCategory + 1, ITEM_CATEGORY_COUNT);
            canvas.setTextColor(TFT_YELLOW);
            canvas.drawString(catTabStr, SCREEN_WIDTH / 2, tabY + 5);

            int catCount = items.getCategoryItemCount(activeCategory);
            if (catCount == 0) {
                canvas.setTextColor(TFT_LIGHTGRAY);
                canvas.drawString("Nenhum item nesta categoria!", SCREEN_WIDTH / 2, 70);
            } else {
                int startItem = max(0, min(subIndex - 1, catCount - 2));
                for (int i = 0; i < 2; i++) {
                    int idxInCat = startItem + i;
                    if (idxInCat >= catCount) break;

                    int itemId = items.getCategoryItemId(activeCategory, idxInCat);
                    const Item& item = items.getItem(itemId);
                    int iy = 38 + (i * 38);

                    if (idxInCat == subIndex) {
                        canvas.fillRoundRect(6, iy, SCREEN_WIDTH - 12, 35, 4, COLOR_ACCENT);
                        canvas.drawRoundRect(6, iy, SCREEN_WIDTH - 12, 35, 4, TFT_YELLOW);
                    } else {
                        canvas.fillRoundRect(6, iy, SCREEN_WIDTH - 12, 35, 4, 0x2104);
                    }

                    anim.drawFood(canvas, 12, iy + 9, item.id);

                    canvas.setTextDatum(TL_DATUM);
                    canvas.setTextColor(TFT_WHITE);
                    canvas.drawString(item.name, 36, iy + 4);

                    canvas.setTextColor(TFT_LIGHTGRAY);
                    canvas.drawString(item.description, 36, iy + 19);

                    canvas.setTextDatum(TR_DATUM);
                    if (isShop) {
                        canvas.setTextColor(COLOR_COIN);
                        char pBuf[16];
                        snprintf(pBuf, sizeof(pBuf), "%d Moedas", item.price);
                        canvas.drawString(pBuf, SCREEN_WIDTH - 12, iy + 10);
                    } else {
                        canvas.setTextColor(TFT_GREEN);
                        char qBuf[16];
                        snprintf(qBuf, sizeof(qBuf), "Qtd: %d", items.getQuantity(item.id));
                        canvas.drawString(qBuf, SCREEN_WIDTH - 12, iy + 10);
                    }
                }
            }

            canvas.setTextDatum(BC_DATUM);
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString(isShop ? "[<-/->] Categoria | [ENTER] Comprar | [ESC] Sair" : "[<-/->] Categoria | [ENTER] Usar | [ESC] Sair", SCREEN_WIDTH / 2, 133);

        } else if (activeSubMenu == SUBMENU_SETTINGS) {
            canvas.drawString("CONFIGURACOES", SCREEN_WIDTH / 2, 10);

            const char* optNames[] = {"Volume", "Brilho Tela", "Timeout Tela", "Sons EFX", "Musica BGM", "Modo Dev", "Config Wi-Fi", "Resetar Save"};
            for (int i = 0; i < 8; i++) {
                int iy = 22 + (i * 13);
                if (i == settingsOption) {
                    canvas.fillRect(6, iy - 1, SCREEN_WIDTH - 12, 12, COLOR_ACCENT);
                    canvas.setTextColor(TFT_YELLOW);
                } else {
                    canvas.setTextColor(TFT_WHITE);
                }

                canvas.setTextDatum(TL_DATUM);
                canvas.drawString(optNames[i], 10, iy);

                canvas.setTextDatum(TR_DATUM);
                char valBuf[48];
                if (i == 0) snprintf(valBuf, sizeof(valBuf), "< %d >", sound.getVolume());
                else if (i == 1) snprintf(valBuf, sizeof(valBuf), "< %d >", brightness);
                else if (i == 2) {
                    if (pet.screenTimeoutSec == 0) snprintf(valBuf, sizeof(valBuf), "< Nunca >");
                    else snprintf(valBuf, sizeof(valBuf), "< %ds >", pet.screenTimeoutSec);
                }
                else if (i == 3) snprintf(valBuf, sizeof(valBuf), "< %s >", sound.isSoundEnabled() ? "LIGADO" : "DESL");
                else if (i == 4) snprintf(valBuf, sizeof(valBuf), "< %s >", sound.isBgmEnabled() ? "LIGADO" : "DESL");
                else if (i == 5) snprintf(valBuf, sizeof(valBuf), "< %s >", clock.getDevMode() ? "LIGADO (30m=1d)" : "DESL");
                else if (i == 6) snprintf(valBuf, sizeof(valBuf), "< %s >", clock.isWifiConnected() ? "ON" : "ENTRAR");
                else if (i == 7) snprintf(valBuf, sizeof(valBuf), "%s", confirmReset ? "CONFIRMAR?" : "[ RESET ]");

                canvas.drawString(valBuf, SCREEN_WIDTH - 10, iy);
            }

            canvas.setTextDatum(BC_DATUM);
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString("[SETAS] Ajustar | [ESC] Sair", SCREEN_WIDTH / 2, 133);

        } else if (activeSubMenu == SUBMENU_WIFI) {
            canvas.drawString("CONFIGURAR WI-FI / NTP", SCREEN_WIDTH / 2, 10);

            canvas.setTextDatum(TL_DATUM);
            canvas.setTextColor(TFT_WHITE);

            // Campo 1: SSID
            canvas.drawString("1. SSID (Rede):", 12, 28);
            if (wifiFieldStep == 0) {
                canvas.fillRoundRect(12, 40, SCREEN_WIDTH - 24, 18, 3, TFT_BLACK);
                canvas.drawRoundRect(12, 40, SCREEN_WIDTH - 24, 18, 3, TFT_YELLOW);
                canvas.setTextColor(TFT_YELLOW);
            } else {
                canvas.fillRoundRect(12, 40, SCREEN_WIDTH - 24, 18, 3, 0x2104);
                canvas.setTextColor(TFT_WHITE);
            }
            char sBuf[48];
            snprintf(sBuf, sizeof(sBuf), "%s%s", wifiSsidBuf, (wifiFieldStep == 0) ? "_" : "");
            canvas.drawString(sBuf, 18, 44);

            // Campo 2: Senha
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString("2. Senha:", 12, 63);
            if (wifiFieldStep == 1) {
                canvas.fillRoundRect(12, 75, SCREEN_WIDTH - 24, 18, 3, TFT_BLACK);
                canvas.drawRoundRect(12, 75, SCREEN_WIDTH - 24, 18, 3, TFT_YELLOW);
                canvas.setTextColor(TFT_YELLOW);
            } else {
                canvas.fillRoundRect(12, 75, SCREEN_WIDTH - 24, 18, 3, 0x2104);
                canvas.setTextColor(TFT_WHITE);
            }
            char pBuf[64];
            snprintf(pBuf, sizeof(pBuf), "%s%s", wifiPassBuf, (wifiFieldStep == 1) ? "_" : "");
            canvas.drawString(pBuf, 18, 79);

            // Status da Conexão
            if (wifiStatusMsg[0] != '\0') {
                canvas.setTextDatum(MC_DATUM);
                canvas.setTextColor(clock.isWifiConnected() ? COLOR_ENERGY : TFT_YELLOW);
                canvas.drawString(wifiStatusMsg, SCREEN_WIDTH / 2, 102);
            }

            canvas.setTextDatum(BC_DATUM);
            canvas.setTextColor(TFT_WHITE);
            canvas.drawString("[ENTER] Conectar | [ESC] Voltar", SCREEN_WIDTH / 2, 132);

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

