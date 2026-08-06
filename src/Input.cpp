#include "Input.h"

InputManager::InputManager() {
    lastKeyPressTime = 0;
    lastAction = ACTION_NONE;
}

void InputManager::update() {
    M5Cardputer.update();
    lastAction = ACTION_NONE;

    uint32_t now = millis();
    if (now - lastKeyPressTime < 120) { // Debounce / Delay entre repetições
        return;
    }

    auto status = M5Cardputer.Keyboard.keysState();

    // 1. CONFIRMAR / SELECIONAR (ENTER no Teclado, Barra de ESPAÇO, Tecla 'E', ou Botão Físico Frontal G0)
    if (status.enter || status.space || M5Cardputer.BtnA.wasPressed() || M5Cardputer.BtnA.isPressed() || M5Cardputer.Keyboard.isKeyPressed('e') || M5Cardputer.Keyboard.isKeyPressed('E')) {
        lastAction = ACTION_SELECT;
        lastKeyPressTime = now;
        return;
    }

    // 2. VOLTAR / ESC (Tecla DEL/BACKSPACE [canto superior esquerdo], TAB, ou Tecla 'Q')
    if (status.del || status.tab || M5Cardputer.Keyboard.isKeyPressed('q') || M5Cardputer.Keyboard.isKeyPressed('Q')) {
        lastAction = ACTION_BACK;
        lastKeyPressTime = now;
        return;
    }

    // 3. NAVEGAÇÃO DE DIREÇÃO (Setas ;, ., ,, / ou Teclas W, A, S, D)
    if (M5Cardputer.Keyboard.isKeyPressed('w') || M5Cardputer.Keyboard.isKeyPressed(';') || M5Cardputer.Keyboard.isKeyPressed('W')) {
        lastAction = ACTION_UP;
        lastKeyPressTime = now;
    } else if (M5Cardputer.Keyboard.isKeyPressed('s') || M5Cardputer.Keyboard.isKeyPressed('.') || M5Cardputer.Keyboard.isKeyPressed('S')) {
        lastAction = ACTION_DOWN;
        lastKeyPressTime = now;
    } else if (M5Cardputer.Keyboard.isKeyPressed('a') || M5Cardputer.Keyboard.isKeyPressed(',') || M5Cardputer.Keyboard.isKeyPressed('A')) {
        lastAction = ACTION_LEFT;
        lastKeyPressTime = now;
    } else if (M5Cardputer.Keyboard.isKeyPressed('d') || M5Cardputer.Keyboard.isKeyPressed('/') || M5Cardputer.Keyboard.isKeyPressed('D')) {
        lastAction = ACTION_RIGHT;
        lastKeyPressTime = now;
    } else if (M5Cardputer.Keyboard.isKeyPressed('1')) {
        lastAction = ACTION_NUM1;
        lastKeyPressTime = now;
    } else if (M5Cardputer.Keyboard.isKeyPressed('2')) {
        lastAction = ACTION_NUM2;
        lastKeyPressTime = now;
    } else if (M5Cardputer.Keyboard.isKeyPressed('3')) {
        lastAction = ACTION_NUM3;
        lastKeyPressTime = now;
    } else if (M5Cardputer.Keyboard.isKeyPressed('4')) {
        lastAction = ACTION_NUM4;
        lastKeyPressTime = now;
    }
}

KeyAction InputManager::getAction() {
    return lastAction;
}

bool InputManager::isPressed(KeyAction action) {
    return lastAction == action;
}
