#include <TWI.h>

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
    // Initialize TWI
    TWI.enable();
    TWI.setFrequency(400000u);
    TWI.setOnTransmitHandler(twiOnTransmit);
    TWI.setOnReceiveHandler(twiOnReceive);

    //TODO read all calibration from slave
    for (uint8_t i = 0; i < 4; i++) {
        TWI.start();
        TWI.writeU08((uint8_t) COMMAND_CALIBRATION_MIN_GET(i));
        TWI.transmit(0xF0);
        //TODO wait OR allow commands fifo buffer in slave
        TWI.receive(0xF0, 2);
        //TODO wait OR allow commands fifo buffer in slave
        min[i] = TWI.readU16();
    }

    while (true) {
        //TODO check buttons & send commands for update state
    }
}
