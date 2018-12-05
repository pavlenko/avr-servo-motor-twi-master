#include <Button.h>
#include <TWI.h>

#include <avr/io.h>

#define COMMAND_CONTROL(_E_)             (0b00010001 & _E_) // 0b0001000E Where E is boolean enable/disable control angle get/set
#define COMMAND_CALIBRATION_MIN_GET(_X_) (0b00100011 & _X_) // 0b00100RXX Where X is channel index, R == 0
#define COMMAND_CALIBRATION_MIN_SET(_X_) (0b00100111 & _X_) // 0b00100WXX Where X is channel index, W == 1
#define COMMAND_CALIBRATION_MAX_GET(_X_) (0b00101011 & _X_) // 0b00101RXX Where X is channel index, R == 0
#define COMMAND_CALIBRATION_MAX_SET(_X_) (0b00101111 & _X_) // 0b00101WXX Where X is channel index, W == 1
#define COMMAND_CALIBRATION_SAVE          0b00100000        // 0b00100000 Store calibration values into EEPROM
#define COMMAND_ANGLE_GET(_X_)           (0b00110011 & _X_) // 0b00110RXX Where X is channel index, R == 0
#define COMMAND_ANGLE_SET(_X_)           (0b00110111 & _X_) // 0b00110WXX Where X is channel index, W == 1
#define COMMAND_MICROSECONDS_GET(_X_)    (0b00111011 & _X_) // 0b00111RXX Where X is channel index, R == 0
#define COMMAND_MICROSECONDS_SET(_X_)    (0b00111111 & _X_) // 0b00111WXX Where X is channel index, W == 1

static uint16_t min[4];
static uint16_t max[4];

void twiOnTransmit() {}

void twiOnReceive() {}

int main() {
    // Initialize buttons
    Button buttonU = Button(&PORTA, PIN0, BUTTON_MODE_PULL_UP);
    Button buttonD = Button(&PORTA, PIN1, BUTTON_MODE_PULL_UP);
    Button buttonL = Button(&PORTA, PIN2, BUTTON_MODE_PULL_UP);
    Button buttonR = Button(&PORTA, PIN3, BUTTON_MODE_PULL_UP);

    // Initialize TWI
    TWI.enable();
    TWI.setFrequency(400000u);
    TWI.setOnTransmitHandler(twiOnTransmit);
    TWI.setOnReceiveHandler(twiOnReceive);

    // Read initial calibration for each servo
    for (uint8_t i = 0; i < 4; i++) {
        TWI.start();
        TWI.writeU08((uint8_t) COMMAND_CALIBRATION_MIN_GET(i));
        TWI.writeU08((uint8_t) COMMAND_CALIBRATION_MAX_GET(i));
        TWI.transmit(0xF0);

        TWI.wait();

        TWI.receive(0xF0, 4);

        TWI.wait();

        min[i] = TWI.readU16();
        max[i] = TWI.readU16();
    }

    uint8_t index = 0;

    while (true) {
        if (BUTTON_STATE_PRESSED == buttonU.getState()) {
            min[index]++;

            TWI.start();
            TWI.writeU08((uint8_t) COMMAND_CALIBRATION_MIN_SET(index));//TODO handle min/max switch
            TWI.writeU16(min[index]);
            TWI.transmit(0xF0);
        } else if (BUTTON_STATE_PRESSED == buttonD.getState()) {
            min[index]--;

            TWI.start();
            TWI.writeU08((uint8_t) COMMAND_CALIBRATION_MIN_SET(index));//TODO handle min/max switch
            TWI.writeU16(min[index]);
            TWI.transmit(0xF0);
        } else if (BUTTON_STATE_PRESSED == buttonL.getState()) {
            index--;//TODO handle index circular changes
        } else if (BUTTON_STATE_PRESSED == buttonR.getState()) {
            index++;//TODO handle index circular changes
        }
    }
}
