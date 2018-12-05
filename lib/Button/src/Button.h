#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#ifndef BUTTON_DEBRIEF_THRESHOLD
#define BUTTON_DEBRIEF_THRESHOLD 100UL
#endif

#ifndef BUTTON_HOLD_THRESHOLD
#define BUTTON_HOLD_THRESHOLD 16000000UL
#endif

enum ButtonMode {
    BUTTON_MODE_PULL_DOWN,
    BUTTON_MODE_PULL_UP,
};

enum ButtonState {
    BUTTON_STATE_RELEASED = -1,
    BUTTON_STATE_UNKNOWN  =  0,
    BUTTON_STATE_PRESSED  =  1,
};

class Button;

typedef void (*ButtonEventHandler_t)(Button &);

class Button {
private:
    volatile uint8_t *_port;
    uint8_t _pin;
    ButtonMode _mode;
    uint8_t _state;
    uint32_t _counterPressed = 0;
    uint8_t _counterReleased = 0;

    ButtonEventHandler_t _onPressHandler;
    ButtonEventHandler_t _onReleaseHandler;
    ButtonEventHandler_t _onHoldHandler;
    uint32_t _onHoldThreshold = BUTTON_HOLD_THRESHOLD;
public:
    /**
     * @param port
     * @param pin
     * @param mode
     */
    Button(volatile uint8_t *port, uint8_t pin, ButtonMode mode);

    /**
     * Get state of button, must be called in loop
     *
     * @return
     */
    ButtonState getState();

    /**
     * Dispatch callbacks, must be called in loop
     */
    void dispatch();

    /**
     * @param handler
     */
    void setOnPressHandler(ButtonEventHandler_t handler);

    /**
     * @param handler
     */
    void setOnReleaseHandler(ButtonEventHandler_t handler);

    /**
     * @param handler
     */
    void setOnHoldHandler(ButtonEventHandler_t handler);

    /**
     * @param threshold
     */
    void setOnHoldThreshold(uint32_t threshold);
};

#endif //BUTTON_H
