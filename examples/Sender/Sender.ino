#include <Arduino.h>

#include <BLERemoteControlSender.h>
#include <NVSRemoteControlStorage.h>

NVSRemoteControlStorage storage;
BLERemoteControlSender sender;

#define GPIO_BUTTON_ON 34
#define GPIO_BUTTON_OFF 35
#define GPIO_BUTTON_SYNC 36

#define PRESSED HIGH

#define COMMAND_LED_ON  0x0001
#define COMMAND_LED_OFF 0x0002


void setup_storage() {
    storage.begin("ble_remote");
    if (!storage.load()) {
        storage.add(new ble_remote_control_info_t(0x0001,true));
        storage.save();
    }
}

void setup_buttons() {
    pinMode(GPIO_BUTTON_ON, INPUT);
    pinMode(GPIO_BUTTON_OFF, INPUT);
    pinMode(GPIO_BUTTON_SYNC, INPUT);
}

void handle_buttons() {
    bool button_on = digitalRead(GPIO_BUTTON_ON);
    bool button_off = digitalRead(GPIO_BUTTON_OFF);
    bool button_sync = digitalRead(GPIO_BUTTON_SYNC);

    if (button_on == PRESSED) sender.send_command(COMMAND_LED_ON, storage[0]);
    if (button_off == PRESSED) sender.send_command(COMMAND_LED_OFF, storage[0]);
    if (button_sync == PRESSED) sender.start_sync(storage[0]);

    if (button_on == PRESSED || button_off == PRESSED || button_sync == PRESSED) {
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