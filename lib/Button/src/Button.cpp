#include "Button.h"

#define BUTTON_BIT_CURRENT    0
#define BUTTON_BIT_PREVIOUS   1
#define BUTTON_BIT_ON_PRESS   2
#define BUTTON_BIT_ON_HOLD    3
#define BUTTON_BIT_ON_RELEASE 4

#define BUTTON_BIT_READ(_byte_, _bit_)           (((_byte_) >> (_bit_)) & 0x01)
#define BUTTON_BIT_SET(_byte_, _bit_)            ((_byte_) |= (1UL << (_bit_)))
#define BUTTON_BIT_CLEAR(_byte_, _bit_)          ((_byte_) &= ~(1UL << (_bit_)))
#define BUTTON_BIT_WRITE(_byte_, _bit_, _value_) (_value_ ? BUTTON_BIT_SET(_byte_, _bit_) : BUTTON_BIT_CLEAR(_byte_, _bit_))

Button::Button(volatile uint8_t *port, uint8_t pin, ButtonMode mode) {
    _port = port;
    _pin  = pin;
    _mode = mode;

    // Set pin as input
    BUTTON_BIT_CLEAR(*(port - 1), pin);

    if (BUTTON_MODE_PULL_UP == mode) {
        // Enable internal pull-up
        BUTTON_BIT_SET(*port, pin);
    }
}

ButtonState Button::getState() {
    // Save the previous value
    BUTTON_BIT_WRITE(_state, BUTTON_BIT_PREVIOUS, BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT));

    // Save the current state (0 == released, 1 == pressed)
    BUTTON_BIT_WRITE(_state, BUTTON_BIT_CURRENT, BUTTON_BIT_READ(*_port, _pin) != _mode);

    if (BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT) != BUTTON_BIT_READ(_state, BUTTON_BIT_PREVIOUS)) {
        // If state changed - reset counters and return unknown state
        _counterPressed  = 0;
        _counterReleased = 0;

        // Clear callback triggered flags
        BUTTON_BIT_CLEAR(_state, BUTTON_BIT_ON_PRESS);
        BUTTON_BIT_CLEAR(_state, BUTTON_BIT_ON_RELEASE);

        return BUTTON_STATE_UNKNOWN;
    } else {
        if (BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT)) {
            if (_counterPressed < sizeof(uint32_t)) {
                _counterPressed++;
            }

            if (_counterPressed < BUTTON_DEBRIEF_THRESHOLD) {
                // If counter less than debrief threshold - increment counter and return unknown state
                return BUTTON_STATE_UNKNOWN;
            }

            return BUTTON_STATE_PRESSED;
        } else {
            if (_counterReleased < BUTTON_DEBRIEF_THRESHOLD) {
                // If counter less than debrief threshold - increment counter and return unknown state
                _counterReleased++;
                return BUTTON_STATE_UNKNOWN;
            }

            return BUTTON_STATE_RELEASED;
        }
    }
}

void Button::dispatch() {
    int8_t state = this->getState();

    if (BUTTON_STATE_PRESSED == state) {
        if (_onPressHandler && !BUTTON_BIT_READ(_state, BUTTON_BIT_ON_PRESS)) {
            _onPressHandler(*this);
            BUTTON_BIT_SET(_state, BUTTON_BIT_ON_PRESS);
        }

        if (_onHoldHandler && !BUTTON_BIT_READ(_state, BUTTON_BIT_ON_HOLD) && _counterPressed >= _onHoldThreshold) {
            _onHoldHandler(*this);
            BUTTON_BIT_SET(_state, BUTTON_BIT_ON_HOLD);
        }
    }

    if (BUTTON_STATE_RELEASED == state) {
        if (_onReleaseHandler && !BUTTON_BIT_READ(_state, BUTTON_BIT_ON_RELEASE)) {
            _onReleaseHandler(*this);
            BUTTON_BIT_SET(_state, BUTTON_BIT_ON_RELEASE);
        }
    }
}

void Button::setOnPressHandler(ButtonEventHandler_t handler) {
    _onPressHandler = handler;
}

void Button::setOnReleaseHandler(ButtonEventHandler_t handler) {
    _onReleaseHandler = handler;
}

void Button::setOnHoldHandler(ButtonEventHandler_t handler) {
    _onHoldHandler = handler;
}

void Button::setOnHoldThreshold(uint32_t threshold) {
    _onHoldThreshold = threshold;
}
