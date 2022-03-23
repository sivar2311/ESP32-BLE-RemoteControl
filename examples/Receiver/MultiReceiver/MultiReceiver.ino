#include <Arduino.h>
#include <BLERemoteControlReceiver.h>
#include <NVSRemoteControlStorage.h>

NVSRemoteControlStorage  storage;
BLERemoteControlReceiver receiver;

#define COMMAND_LED_ON  0x0001
#define COMMAND_LED_OFF 0x0002

#define GPIO_LED         16
#define GPIO_SYNC_BUTTON 34
#define PRESSED          HIGH

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

void handle_sync_button() {
    bool sync_button = digitalRead(GPIO_SYNC_BUTTON);

    if (sync_button == PRESSED) {
        if (receiver.is_synchronizing()) {
            receiver.stop_synchronizing();
        } else {
            receiver.start_synchronizing();
        }
        delay(150);  // simple debounce
    }
}

void setup_led() {
    pinMode(GPIO_LED, OUTPUT);
}

void setup_button() {
    pinMode(GPIO_SYNC_BUTTON, INPUT);
}

void setup_receiver() {
    storage.begin("ble_remote");
    storage.load();

    receiver.on_command(handle_command);
    receiver.set_sync_pin(123456);
    receiver.begin(&storage);
}

void setup() {
    setup_led();
    setup_button();
    setup_receiver();
}

void loop() {
    handle_sync_button();
}