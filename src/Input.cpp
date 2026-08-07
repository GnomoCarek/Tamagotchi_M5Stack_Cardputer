#include "Input.h"

InputManager::InputManager() {
    for (int i = 0; i < ACTION_COUNT; i++) {
        rawState[i] = false;
        justPressedState[i] = false;
        repeatPressedState[i] = false;
        lastRawState[i] = false;
        actionPressTime[i] = 0;
        actionRepeatTime[i] = 0;
    }
    typedChar = '\0';
    lastChar = '\0';
    charPressTime = 0;
    charRepeatTime = 0;
}

void InputManager::update() {
    M5Cardputer.update();
    uint32_t now = millis();

    // Reset instant flags for this frame
    for (int i = 0; i < ACTION_COUNT; i++) {
        justPressedState[i] = false;
        repeatPressedState[i] = false;
        rawState[i] = false;
    }

    const auto& keys = M5Cardputer.Keyboard.keyList();
    auto status = M5Cardputer.Keyboard.keysState();

    // Botão Físico Frontal G0 (BtnA)
    if (M5Cardputer.BtnA.isPressed()) {
        rawState[ACTION_SELECT] = true;
    }

    // Leitura por coordenadas físicas da matriz do Cardputer / Cardputer ADV
    for (const auto& pos : keys) {
        int x = pos.x;
        int y = pos.y;

        // Navegação de Direção:
        // CIMA: tecla ';' (x=11, y=2) ou 'W' (x=2, y=1)
        if ((x == 11 && y == 2) || (x == 2 && y == 1)) rawState[ACTION_UP] = true;

        // BAIXO: tecla '.' (x=11, y=3) ou 'S' (x=3, y=2)
        if ((x == 11 && y == 3) || (x == 3 && y == 2)) rawState[ACTION_DOWN] = true;

        // ESQUERDA: tecla ',' (x=10, y=3) ou 'A' (x=2, y=2)
        if ((x == 10 && y == 3) || (x == 2 && y == 2)) rawState[ACTION_LEFT] = true;

        // DIREITA: tecla '/' (x=12, y=3) ou 'D' (x=4, y=2)
        if ((x == 12 && y == 3) || (x == 4 && y == 2)) rawState[ACTION_RIGHT] = true;

        // SELECT / ENTER: tecla Enter (x=13, y=2)
        if (x == 13 && y == 2) rawState[ACTION_SELECT] = true;

        // ESPAÇO: barra de espaço (x=13, y=3)
        if (x == 13 && y == 3) rawState[ACTION_SPACE] = true;

        // BACK: Tab (x=0, y=1) ou Q (x=1, y=1)
        if ((x == 0 && y == 1) || (x == 1 && y == 1)) rawState[ACTION_BACK] = true;

        // DELETE: Backspace (x=13, y=0)
        if (x == 13 && y == 0) rawState[ACTION_DELETE] = true;

        // Números 1, 2, 3, 4 (linha y=0)
        if (y == 0 && x == 1) rawState[ACTION_NUM1] = true;
        if (y == 0 && x == 2) rawState[ACTION_NUM2] = true;
        if (y == 0 && x == 3) rawState[ACTION_NUM3] = true;
        if (y == 0 && x == 4) rawState[ACTION_NUM4] = true;
    }

    // Suporte complementar via status de flags do teclado
    if (status.enter) rawState[ACTION_SELECT] = true;
    if (status.space) rawState[ACTION_SPACE] = true;
    if (status.del) rawState[ACTION_DELETE] = true;
    if (status.tab) rawState[ACTION_BACK] = true;

    // Processamento confiável de debounce e repetição
    for (int i = 1; i < ACTION_COUNT; i++) {
        if (rawState[i]) {
            if (!lastRawState[i]) {
                // Primeiro toque!
                justPressedState[i] = true;
                repeatPressedState[i] = true;
                actionPressTime[i] = now;
                actionRepeatTime[i] = now;
            } else {
                // Mantido pressionado: repete após 450ms, a cada 120ms
                if ((now - actionPressTime[i] >= 450) && (now - actionRepeatTime[i] >= 120)) {
                    repeatPressedState[i] = true;
                    actionRepeatTime[i] = now;
                }
            }
        }
        lastRawState[i] = rawState[i];
    }

    // Processamento de caracteres digitados para entrada de texto (Nome do Pet)
    typedChar = '\0';
    char currentChar = '\0';

    if (!status.word.empty()) {
        currentChar = status.word[0];
    } else if (status.space) {
        currentChar = ' ';
    } else if (status.del) {
        currentChar = '\b';
    }

    if (currentChar != '\0') {
        if (currentChar != lastChar) {
            typedChar = currentChar;
            charPressTime = now;
            charRepeatTime = now;
        } else {
            if ((now - charPressTime >= 450) && (now - charRepeatTime >= 120)) {
                typedChar = currentChar;
                charRepeatTime = now;
            }
        }
    }
    lastChar = currentChar;
}

bool InputManager::isJustPressed(KeyAction action) const {
    if (action <= ACTION_NONE || action >= ACTION_COUNT) return false;
    return justPressedState[action];
}

bool InputManager::repeatPressed(KeyAction action) const {
    if (action <= ACTION_NONE || action >= ACTION_COUNT) return false;
    return repeatPressedState[action];
}

bool InputManager::isPressed(KeyAction action) const {
    return repeatPressed(action);
}

bool InputManager::isHeld(KeyAction action) const {
    if (action <= ACTION_NONE || action >= ACTION_COUNT) return false;
    return rawState[action];
}

bool InputManager::isReleased(KeyAction action) const {
    if (action <= ACTION_NONE || action >= ACTION_COUNT) return false;
    return (!rawState[action] && lastRawState[action]);
}

KeyAction InputManager::getAction() {
    for (int i = 1; i < ACTION_COUNT; i++) {
        if (repeatPressedState[i]) {
            return (KeyAction)i;
        }
    }
    return ACTION_NONE;
}


