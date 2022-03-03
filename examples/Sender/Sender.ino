#include <Arduino.h>

#include "BLERemoteControlSender.h"

BLERemoteControlSender sender;

#define GPIO_BUTTON_1 34
#define GPIO_BUTTON_2 35

#define PRESSED HIGH

#define REMOTE_ID 0x0001

#define COMMAND_LED_ON  0x0001
#define COMMAND_LED_OFF 0x0002

ble_remote_control_info_t remote_control(REMOTE_ID, {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F});

void setup_buttons() {
    pinMode(GPIO_BUTTON_1, INPUT);
    pinMode(GPIO_BUTTON_2, INPUT);
}

void handle_buttons() {
    bool button_1 = digitalRead(GPIO_BUTTON_1);
    bool button_2 = digitalRead(GPIO_BUTTON_2);

    if (button_1 == PRESSED) sender.send_command(COMMAND_LED_ON, &remote_control);
    if (button_2 == PRESSED) sender.send_command(COMMAND_LED_OFF, &remote_control);

    if (button_1 == PRESSED || button_2 == PRESSED) {
        delay(150); // simple "debounce"
    }
}

void setup() {
    setup_buttons();
    sender.begin();
}

void loop() {
    handle_buttons();
}