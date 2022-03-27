#include <Arduino.h>
#include <BLERemoteControlReceiver.h>
#include <NVSRemoteControlStorage.h>

NVSRemoteControlStorage  storage;
BLERemoteControlReceiver receiver;

const uint32_t COMMAND_LED_ON  = 0x0001;
const uint32_t COMMAND_LED_OFF = 0x0002;

const int GPIO_LED         = 16;
const int GPIO_SYNC_BUTTON = 34;

const bool PRESSED = HIGH;

const uint32_t SHARED_SYNCHRONIZATION_PIN = 123456;

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
        receiver.start_synchronizing(10);
    }

    delay(150);  // simple button debounce
}

void setup_led() {
    pinMode(GPIO_LED, OUTPUT);
}

void setup_button() {
    pinMode(GPIO_SYNC_BUTTON, INPUT);
}

void setup_storage() {
    storage.begin("ble_remote");
}

void setup_receiver() {
    receiver.on_command(handle_command);
    receiver.begin(&storage, SHARED_SYNCHRONIZATION_PIN);
}

void setup() {
    setup_led();
    setup_button();
    setup_storage();
    setup_receiver();
}

void loop() {
    handle_sync_button();
}