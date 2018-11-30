#include "Button.h"

Button::Button(volatile uint8_t *port, volatile uint8_t *ddr, uint8_t pin, ButtonMode mode) {
    _mode = mode;
}

bool Button::isPressed() {
    //save the previous value
    bitWrite(state, PREVIOUS, bitRead(state, CURRENT));

    //get the current status of the pin
    if (digitalRead(pin) == _mode) {
        //currently the button is not pressed
        bitWrite(state, CURRENT, false);
    } else {
        //currently the button is pressed
        bitWrite(state, CURRENT, true);
    }
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