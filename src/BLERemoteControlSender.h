#pragma once

#include <NimBLEDevice.h>

#include "BLERemoteControlBase.h"
#include "BLERemoteControlStorage.h"

class BLERemoteControlSender : public BLERemoteControlBaseClass, protected NimBLEServerCallbacks {
  public:
    void begin(uint32_t id, BLERemoteControlStorage* storage, uint32_t pin);

    void send_command(uint32_t command);

    void start_synchronizing(int seconds);
    void stop_synchronizing();
    bool is_synchronizing();

  protected:
    BLERemoteControlStorage* get_storage();
    virtual void             onDisconnect(NimBLEServer* pServer) override;

  protected:
    uint32_t id = 0x0001;

    int           synchronize_timeout     = 5;
    unsigned long synchronizing_timestamp = 0;

    NimBLEAdvertising* ble_advertising = nullptr;

    NimBLEAdvertisementData sync_adv_data;

    NimBLEServer*         ble_server         = nullptr;
    NimBLEService*        ble_service        = nullptr;
    NimBLECharacteristic* ble_characteristic = nullptr;

    BLERemoteControlStorage* storage = nullptr;
};

void BLERemoteControlSender::begin(uint32_t id, BLERemoteControlStorage* storage, uint32_t pin) {
    NimBLEDevice::init("");

    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityPasskey(pin);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

    this->id      = id;
    this->storage = storage;

    ble_server = NimBLEDevice::createServer();
    ble_server->advertiseOnDisconnect(false);
    ble_server->setCallbacks(this);

    ble_service        = ble_server->createService(NimBLEUUID(remote_control_service_uuid));
    ble_characteristic = ble_service->createCharacteristic(NimBLEUUID(remote_control_characteristic_uuid), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN);
    ble_service->start();

    ble_advertising = NimBLEDevice::getAdvertising();
    ble_advertising->setMinInterval(0x20);
    ble_advertising->setMaxInterval(0xA0);

    sync_adv_data.setFlags(0x06);
    sync_adv_data.setCompleteServices(NimBLEUUID(remote_control_service_uuid));
}

void BLERemoteControlSender::send_command(uint32_t command) {
    ble_remote_control_info_t remote_control;
    if (!get_storage()->load(id, &remote_control)) return;

    if (is_synchronizing()) stop_synchronizing();

    BLEManufacturerData man_data;
    man_data.company_id = 0xFFFF;
    man_data.remote_id  = remote_control.id;

    BLERemoteControlMessage message;
    message.remote_id    = remote_control.id;
    message.command      = command;
    message.rolling_code = remote_control.rolling_code++;
    message.nonce        = random(0xFFFF);
    get_storage()->save(&remote_control);

    encrypt_message(man_data.encrypted_message, &message, remote_control.aes_key);

    NimBLEAdvertisementData adv_data;

    adv_data.setFlags(0x00);
    adv_data.setManufacturerData(std::string((const char*)&man_data, sizeof(BLEManufacturerData)));

    ble_advertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);
    ble_advertising->setAdvertisementData(adv_data);

    ble_advertising->start(1);
}

void BLERemoteControlSender::start_synchronizing(int seconds) {
    synchronize_timeout = (seconds * 1000);

    ble_remote_control_info_t remote_control;
    if (!get_storage()->load(id, &remote_control)) return;

    ble_characteristic->setValue(remote_control);
    remote_control.rolling_code++;
    get_storage()->save(&remote_control);

    ble_advertising->setAdvertisementType(BLE_GAP_CONN_MODE_UND);
    ble_advertising->setAdvertisementData(sync_adv_data);
    ble_advertising->start(seconds);
    synchronizing_timestamp = millis();
}

void BLERemoteControlSender::stop_synchronizing() {
    ble_advertising->stop();
    synchronizing_timestamp = 0;
}

bool BLERemoteControlSender::is_synchronizing() {
    unsigned long current_millis = millis();

    bool _synchronize_timed_out = (synchronizing_timestamp && synchronize_timeout && current_millis - synchronizing_timestamp >= synchronize_timeout);
    if (_synchronize_timed_out) stop_synchronizing();

    bool _is_synchronizing = (synchronizing_timestamp > 0);
    return _is_synchronizing;
}

BLERemoteControlStorage* BLERemoteControlSender::get_storage() {
    if (storage == nullptr) storage = new BLERemoteControlStorage();
    return storage;
}

void BLERemoteControlSender::onDisconnect(NimBLEServer* pServer) {
    if (synchronize_timeout) {
        if (is_synchronizing()) stop_synchronizing();
    } else {
        start_synchronizing(0);
    }
}