#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include <M5Cardputer.h>

enum KeyAction {
    ACTION_NONE = 0,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_SELECT,
    ACTION_BACK,
    ACTION_NUM1,
    ACTION_NUM2,
    ACTION_NUM3,
    ACTION_NUM4
};

class InputManager {
private:
    uint32_t lastKeyPressTime;
    KeyAction lastAction;

public:
    InputManager();

    void update();
    KeyAction getAction();
    bool isPressed(KeyAction action);
};

#endif // INPUT_H
