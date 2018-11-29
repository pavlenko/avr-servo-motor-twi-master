#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#ifndef BUTTON_DEBRIEF_ITERATIONS
#define BUTTON_DEBRIEF_ITERATIONS 10
#endif

class Button;

typedef void (*ButtonEventHandler_t)(Button&);

class Button {
private:
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t pin;

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
    Button(volatile uint8_t *port, volatile uint8_t *ddr, uint8_t pin, uint8_t mode);

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
