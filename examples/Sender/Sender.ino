#include <Arduino.h>
#include <BLERemoteControlSender.h>
#include <NVSRemoteControlStorage.h>

NVSRemoteControlStorage storage;
BLERemoteControlSender  sender;

const int GPIO_BUTTON_ON   = 34;
const int GPIO_BUTTON_OFF  = 35;
const int GPIO_BUTTON_SYNC = 36;

const bool PRESSED = HIGH;

const uint32_t COMMAND_LED_ON  = 0x0001;
const uint32_t COMMAND_LED_OFF = 0x0002;

const uint32_t UNIQUE_REMOTE_ID           = 0xBEEF;
const int      SHARED_SYNCHRONIZATION_PIN = 123456;
const int      SYNCHRONIZATION_TIMEOUT    = 10;

void handle_buttons() {
    bool button_on   = digitalRead(GPIO_BUTTON_ON);
    bool button_off  = digitalRead(GPIO_BUTTON_OFF);
    bool button_sync = digitalRead(GPIO_BUTTON_SYNC);

    if (button_on == PRESSED) sender.send_command(COMMAND_LED_ON);
    if (button_off == PRESSED) sender.send_command(COMMAND_LED_OFF);
    if (button_sync == PRESSED) sender.start_synchronizing(SYNCHRONIZATION_TIMEOUT);

    bool a_button_was_pressed = (button_on == PRESSED || button_off == PRESSED || button_sync == PRESSED);
    if (a_button_was_pressed) {
        delay(150);  // simple "debounce"
    }
}

void setup_buttons() {
    pinMode(GPIO_BUTTON_ON, INPUT);
    pinMode(GPIO_BUTTON_OFF, INPUT);
    pinMode(GPIO_BUTTON_SYNC, INPUT);
}

void setup_storage() {
    storage.begin("ble_remote");

    if (storage.exist(UNIQUE_REMOTE_ID) == false) {
        storage.add(new ble_remote_control_info_t(UNIQUE_REMOTE_ID, true));
        storage.save();
    }
}

void setup_sender() {
    sender.begin(UNIQUE_REMOTE_ID, &storage, SHARED_SYNCHRONIZATION_PIN);
}

void setup() {
    setup_buttons();
    setup_storage();
    setup_sender();
}

void loop() {
    handle_buttons();
}