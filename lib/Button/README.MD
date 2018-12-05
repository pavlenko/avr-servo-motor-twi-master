# Button library for ATMega devices

This library allows to handle button interactions

## Usage

```cpp
#include <Button.h>

#include <avr/io.h>

void onPress(Button &button) {
    // Do some stuff
}

void onRelease(Button &button) {
    // Do some stuff
}

int main() {
    // Initialize buttons
    Button button1 = Button(&PORTA, PIN0, BUTTON_MODE_PULL_UP);   //<-- Button connected to GND
    Button button2 = Button(&PORTA, PIN1, BUTTON_MODE_PULL_DOWN); //<-- Button connected to VCC

    // You can set handler callbacks for dispatch later
    button2.setOnPressHandler(onPress);
    button2.setOnReleaseHandler(onRelease);

    while (true) {
        // Now You can check button state and execute some code depends on state
        if (BUTTON_STATE_PRESSED == button1.getState()) {
            // Do some stuff
        }

        // Or You can dispatch callbacks for get your code more clear
        button2.dispatch();
    }
}
```

## TODO

- Long press handler
- Repeated long press callback execution