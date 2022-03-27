#pragma once

#include <Arduino.h>
#include <NimBLEDevice.h>

#include "BLERemoteControlBase.h"
#include "BLERemoteControlInfo.h"
#include "BLERemoteControlStorage.h"

struct ble_remote_command_info_t {
    ble_remote_command_info_t(uint32_t command, ble_remote_control_info_t* remote_control)
        : command(command), remote_control(remote_control) {}
    uint32_t                   command;
    ble_remote_control_info_t* remote_control;
};

using ble_receive_callback_t     = std::function<void(ble_remote_command_info_t*)>;
using ble_remote_sync_callback_t = std::function<void(ble_remote_control_info_t*)>;

class BLERemoteControlReceiver : public BLERemoteControlBaseClass, public NimBLEAdvertisedDeviceCallbacks, public NimBLEClientCallbacks {
  public:
    BLERemoteControlReceiver();
    ~BLERemoteControlReceiver();

    void begin(BLERemoteControlStorage* storage, uint32_t pin);
    void on_command(ble_receive_callback_t callback);
    void on_synchronize(ble_remote_sync_callback_t callback);

    void start_synchronizing(int seconds);
    void stop_synchronizing();
    bool is_synchronizing();

    void handle();

  protected:
    void onResult(NimBLEAdvertisedDevice* advertised_device);

    void process_manufacturer_data(BLEManufacturerData* data);
    void process_command(BLERemoteControlMessage* message, ble_remote_control_info_t* remote_control);
    bool check_rolling_code(BLERemoteControlMessage* message, ble_remote_control_info_t* remote_control);

    void process_service(NimBLEAdvertisedDevice* advertised_device);
    void synchronize_device(NimBLEAddress* address);
    void check_sync_timeout();

    uint32_t onPassKeyRequest();

    BLERemoteControlStorage* get_storage();

  protected:
    NimBLEScan*              ble_scan = nullptr;
    BLERemoteControlStorage* _storage;

    ble_receive_callback_t     receive_callback   = nullptr;
    ble_remote_sync_callback_t sync_done_callback = nullptr;

    NimBLEAddress* sync_device_address;

    uint32_t      pin            = 123456;
    unsigned long sync_timestamp = 0;
    unsigned long sync_timeout   = 10000;
};

BLERemoteControlReceiver::BLERemoteControlReceiver() {}
BLERemoteControlReceiver::~BLERemoteControlReceiver() {}

void BLERemoteControlReceiver::begin(BLERemoteControlStorage* storage, uint32_t pin) {
    this->_storage = storage;
    this->pin      = pin;

    NimBLEDevice::setScanDuplicateCacheSize(10);
    NimBLEDevice::init("");
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY);

    ble_scan = NimBLEDevice::getScan();
    ble_scan->setAdvertisedDeviceCallbacks(this, true);
    ble_scan->setActiveScan(false);
    ble_scan->setInterval(20);
    ble_scan->setWindow(10);
    ble_scan->setMaxResults(0);
    ble_scan->setDuplicateFilter(false);
    ble_scan->start(0, nullptr, false);
}

void BLERemoteControlReceiver::on_command(ble_receive_callback_t callback) {
    receive_callback = callback;
}

void BLERemoteControlReceiver::start_synchronizing(int seconds) {
    sync_timestamp = millis();
    sync_timeout   = ((unsigned long)seconds) * 1000;
};

void BLERemoteControlReceiver::stop_synchronizing() {
    sync_timestamp = 0;
};

bool BLERemoteControlReceiver::is_synchronizing() {
    unsigned long current_millis = millis();

    bool _synchronize_timed_out = (sync_timestamp && sync_timeout && current_millis - sync_timestamp >= sync_timeout);

    if (_synchronize_timed_out) {
        stop_synchronizing();
        if (sync_done_callback) sync_done_callback(nullptr);
    }

    bool _is_synchronizing = (sync_timestamp > 0);
    return _is_synchronizing;
}

void BLERemoteControlReceiver::on_synchronize(ble_remote_sync_callback_t callback) {
    sync_done_callback = callback;
}

void BLERemoteControlReceiver::onResult(NimBLEAdvertisedDevice* advertised_device) {
    if (advertised_device->haveManufacturerData()) {
        BLEManufacturerData manufacturer_data = advertised_device->getManufacturerData<BLEManufacturerData>();
        if (manufacturer_data.company_id == 0xFFFF) process_manufacturer_data(&manufacturer_data);
    }

    if (advertised_device->haveServiceUUID()) {
        process_service(advertised_device);
    }
};

void BLERemoteControlReceiver::process_manufacturer_data(BLEManufacturerData* data) {
    BLERemoteControlStorage* storage = get_storage();

    auto                      remote_id = data->remote_id;
    ble_remote_control_info_t remote_control;
    if (storage->load(remote_id, &remote_control) == nullptr) return;

    BLERemoteControlMessage message;
    decrypt_message(&message, data->encrypted_message, remote_control.aes_key);
    if (message.remote_id != remote_id) return;

    process_command(&message, &remote_control);
}

void BLERemoteControlReceiver::process_command(BLERemoteControlMessage* message, ble_remote_control_info_t* remote_control) {
    auto command = message->command;

    if (check_rolling_code(message, remote_control)) {
        if (receive_callback) {
            ble_remote_command_info_t ci(command, remote_control);
            receive_callback(&ci);
            get_storage()->save(remote_control);
        }
    }
}

bool BLERemoteControlReceiver::check_rolling_code(BLERemoteControlMessage* message, ble_remote_control_info_t* remote_control) {
    auto old_rolling_code = remote_control->rolling_code;
    auto new_rolling_code = message->rolling_code;

    if (new_rolling_code > old_rolling_code) {
        remote_control->rolling_code = new_rolling_code;
        return true;
    }

    return false;
}

void BLERemoteControlReceiver::process_service(NimBLEAdvertisedDevice* advertised_device) {
    if (!is_synchronizing()) return;

    if (advertised_device->isAdvertisingService(NimBLEUUID(remote_control_service_uuid))) {
        sync_device_address = new NimBLEAddress(advertised_device->getAddress());
    }
}

void BLERemoteControlReceiver::synchronize_device(NimBLEAddress* address) {
    BLERemoteControlStorage* storage = get_storage();

    NimBLEClient* pClient = NimBLEDevice::createClient();
    if (!pClient) return;

    pClient->setClientCallbacks(this);

    if (pClient->connect(*address)) {
        pClient->secureConnection();

        NimBLERemoteService* pService = pClient->getService(NimBLEUUID(remote_control_service_uuid));
        if (pService != nullptr) {
            NimBLERemoteCharacteristic* pSecureCharacteristic = pService->getCharacteristic(NimBLEUUID(remote_control_characteristic_uuid));

            if (pSecureCharacteristic != nullptr) {
                ble_remote_control_info_t remote_control = pSecureCharacteristic->readValue<ble_remote_control_info_t>();
                storage->save(&remote_control);
                if (sync_timeout) stop_synchronizing();
                if (sync_done_callback) sync_done_callback(&remote_control);
            }

            pClient->disconnect();
        }
    }
    ble_scan->start(0, nullptr, false);
}

uint32_t BLERemoteControlReceiver::onPassKeyRequest() {
    return pin;
}

BLERemoteControlStorage* BLERemoteControlReceiver::get_storage() {
    if (_storage == nullptr) _storage = new BLERemoteControlStorage();
    return _storage;
}

void BLERemoteControlReceiver::handle() {
    check_sync_timeout();

    if (is_synchronizing() && sync_device_address != nullptr) {
        synchronize_device(sync_device_address);
        delete sync_device_address;
        sync_device_address = nullptr;
    }
}

void BLERemoteControlReceiver::check_sync_timeout() {
}