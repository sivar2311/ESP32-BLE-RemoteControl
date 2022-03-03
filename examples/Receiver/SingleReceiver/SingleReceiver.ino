#include <Arduino.h>

#include "BLERemoteControlReceiver.h"

BLERemoteControlReceiver receiver;

#define COMMAND_LED_ON  0x0001
#define COMMAND_LED_OFF 0x0002

#define REMOTE_ID 0x0001

#define GPIO_LED 16

ble_remote_control_info_t remote(REMOTE_ID, {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F});

void turn_on_led() {
    digitalWrite(GPIO_LED, HIGH);
}

void turn_off_led() {
    digitalWrite(GPIO_LED, LOW);
}

void handle_command(ble_remote_command_info_t* command_info) {
    uint16_t command = command_info->command;

    if (command == COMMAND_LED_ON) turn_on_led();
    if (command == COMMAND_LED_OFF) turn_off_led();
}

void setup_led() {
    pinMode(GPIO_LED, OUTPUT);
}

void setup_receiver() {
    receiver.on_command(handle_command);

    receiver.add_remote_control(&remote);
    receiver.begin();
}

void setup() {
    setup_led();
    setup_receiver();
}

void loop() {
}