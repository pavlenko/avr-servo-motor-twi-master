#include "Button.h"

#include <avr/sfr_defs.h>

Button::Button(volatile uint8_t *port, volatile uint8_t *ddr, uint8_t pin, ButtonMode mode) {
    _port = port;
    _pin  = pin;
    _mode = mode;

    // Set pin as input
    *ddr &= _BV(pin);

    if (BUTTON_MODE_PULL_UP == mode) {
        // Enable internal pull-up
        *port |= _BV(pin);
    }
}

bool Button::isPressed() {
    //save the previous value
    BUTTON_BIT_WRITE(_state, BUTTON_BIT_PREVIOUS, BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT));

    //get the current status of the pin
    if (BUTTON_BIT_READ(*_port, _pin) == _mode) {
        //currently the button is not pressed
        BUTTON_BIT_WRITE(_state, BUTTON_BIT_CURRENT, false);
    } else {
        //currently the button is pressed
        BUTTON_BIT_WRITE(_state, BUTTON_BIT_CURRENT, true);
    }

    //handle state changes
    if (BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT) != BUTTON_BIT_READ(_state, BUTTON_BIT_PREVIOUS)) {
        if (BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT)) {
            //the state changed to PRESSED
            //TODO count pressed

            numberOfPresses++;

            if (_onPress) {
                _onPress(*this);
            }

            pressedStartTime   = millis();             //start timing
            triggeredHoldEvent = false;
        } else {
            //the state changed to RELEASED
            if (_onRelease) {
                _onRelease(*this);
            }

            if (_onClick) {
                _onClick(*this);
            }

            //reset states (for timing and for event triggering)
            pressedStartTime = -1;
        }

        //note that the state changed
        BUTTON_BIT_WRITE(_state, BUTTON_BIT_CHANGED, true);
    } else {
        //note that the state did not change
        BUTTON_BIT_WRITE(_state, BUTTON_BIT_CHANGED, false);

        //should we trigger a onHold event?
        if (pressedStartTime != -1 && !triggeredHoldEvent && millis() - pressedStartTime > holdEventThreshold) {
            if (_onHold) {
                _onHold(*this);
                triggeredHoldEvent = true;
            }
        }
    }

    return (bool) BUTTON_BIT_READ(_state, BUTTON_BIT_CURRENT);
}

void Button::setOnPressHandler(ButtonEventHandler_t handler) {
    _onPress = handler;
}

void Button::setOnReleaseHandler(ButtonEventHandler_t handler) {
    _onRelease = handler;
}

void Button::setOnClickHandler(ButtonEventHandler_t handler) {
    _onClick = handler;
}

void Button::setOnHoldHandler(ButtonEventHandler_t handler) {
    _onHold = handler;
}