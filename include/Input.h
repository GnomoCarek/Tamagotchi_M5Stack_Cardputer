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
    ACTION_SPACE,
    ACTION_BACK,
    ACTION_DELETE,
    ACTION_NUM1,
    ACTION_NUM2,
    ACTION_NUM3,
    ACTION_NUM4,
    ACTION_COUNT
};

class InputManager {
private:
    bool rawState[ACTION_COUNT];
    bool justPressedState[ACTION_COUNT];
    bool repeatPressedState[ACTION_COUNT];
    bool lastRawState[ACTION_COUNT];
    uint32_t actionPressTime[ACTION_COUNT];
    uint32_t actionRepeatTime[ACTION_COUNT];

    char typedChar;
    char lastChar;
    uint32_t charPressTime;
    uint32_t charRepeatTime;

public:
    InputManager();

    void update();

    bool isJustPressed(KeyAction action) const;
    bool repeatPressed(KeyAction action) const;
    bool isPressed(KeyAction action) const; // Alias for repeatPressed
    bool isHeld(KeyAction action) const;
    bool isReleased(KeyAction action) const;

    KeyAction getAction();
    char getTypedChar() const { return typedChar; }
};

#endif // INPUT_H

