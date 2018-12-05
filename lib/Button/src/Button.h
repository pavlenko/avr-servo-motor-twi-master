#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#ifndef BUTTON_DEBRIEF_ITERATIONS
#define BUTTON_DEBRIEF_ITERATIONS 10
#endif

#define BUTTON_BIT_CURRENT  0
#define BUTTON_BIT_PREVIOUS 1
#define BUTTON_BIT_CHANGED  2

#define BUTTON_STATE_RELEASED -1
#define BUTTON_STATE_UNKNOWN   0
#define BUTTON_STATE_PRESSED   1

#define BUTTON_BIT_READ(_byte_, _bit_)           (((_byte_) >> (_bit_)) & 0x01)
#define BUTTON_BIT_SET(_byte_, _bit_)            ((_byte_) |= (1UL << (_bit_)))
#define BUTTON_BIT_CLEAR(_byte_, _bit_)          ((_byte_) &= ~(1UL << (_bit_)))
#define BUTTON_BIT_WRITE(_byte_, _bit_, _value_) (_value_ ? BUTTON_BIT_SET(_byte_, _bit_) : BUTTON_BIT_CLEAR(_byte_, _bit_))

enum ButtonMode {
    BUTTON_MODE_PULL_DOWN,
    BUTTON_MODE_PULL_UP,
};

class Button;

typedef void (*ButtonEventHandler_t)(Button&);

class Button {
private:
    volatile uint8_t *_port;
    uint8_t _pin;
    ButtonMode _mode;
    uint8_t _state;
    uint8_t _counterPressed  = 0;
    uint8_t _counterReleased = 0;

    ButtonEventHandler_t _onPress;
    ButtonEventHandler_t _onRelease;
    ButtonEventHandler_t _onClick;
    ButtonEventHandler_t _onHold;
public:
    /**
     * @param port
     * @param ddr
     * @param pin
     * @param mode
     */
    Button(volatile uint8_t *port, volatile uint8_t *ddr, uint8_t pin, ButtonMode mode);

    int8_t getState() {
        // Save the previous value
        BUTTON_BIT_WRITE(_state, BUTTON_BIT_PREVIOUS, BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT));

        // Save the current state (0 == released, 1 == pressed)
        BUTTON_BIT_WRITE(_state, BUTTON_BIT_CURRENT, BUTTON_BIT_READ(*_port, _pin) != _mode);

        if (BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT) != BUTTON_BIT_READ(_state, BUTTON_BIT_PREVIOUS)) {
            // If state changed - reset counters and return unknown state
            _counterPressed  = 0;
            _counterReleased = 0;
            return BUTTON_STATE_UNKNOWN;
        } else {
            if (BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT)) {
                if (_counterPressed < BUTTON_DEBRIEF_ITERATIONS) {
                    // If counter less than debrief threshold - increment counter and return unknown state
                    _counterPressed++;
                    return BUTTON_STATE_UNKNOWN;
                }

                return BUTTON_STATE_PRESSED;
            } else {
                if (_counterReleased < BUTTON_DEBRIEF_ITERATIONS) {
                    // If counter less than debrief threshold - increment counter and return unknown state
                    _counterReleased++;
                    return BUTTON_STATE_UNKNOWN;
                }

                return BUTTON_STATE_RELEASED;
            }
        }
    }

    void dispatch() {
        //TODO dispatch onPress and onRelease as singular events
        int8_t state = this->getState();

        if (BUTTON_STATE_PRESSED == state && _onPress) {
            _onPress(*this);
        }
        if (BUTTON_STATE_RELEASED == state && _onRelease) {
            _onRelease(*this);
        }
    }

    /**
     * @return
     */
    bool isPressed();

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
    void setOnClickHandler(ButtonEventHandler_t handler);

    /**
     * @param handler
     */
    void setOnHoldHandler(ButtonEventHandler_t handler);
};

#endif //BUTTON_H
