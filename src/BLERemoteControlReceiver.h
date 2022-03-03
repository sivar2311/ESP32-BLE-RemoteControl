#pragma once

#include <Arduino.h>
#include <NimBLEDevice.h>

#include "BLERemoteControlBase.h"
#include "BLERemoteControlInfo.h"

struct ble_remote_command_info_t {
    uint16_t                   command;
    ble_remote_control_info_t* remote_control;
};

using ble_receive_callback_t     = std::function<void(ble_remote_command_info_t*)>;
using ble_remote_sync_callback_t = std::function<void(ble_remote_control_info_t*)>;

class BLERemoteControlReceiver : public BLERemoteControlBaseClass, public NimBLEAdvertisedDeviceCallbacks {
  public:
    BLERemoteControlReceiver();
    ~BLERemoteControlReceiver();
    void begin();

    void add_remote_control(ble_remote_control_info_t* remote_control);
    void remove_remote_control(ble_remote_control_info_t* remote_control);

    void on_command(ble_receive_callback_t callback);

    void sync_remotecontrol();
    void on_sync(ble_remote_sync_callback_t callback);

  protected:
    void                       onResult(NimBLEAdvertisedDevice* advertised_device);
    ble_remote_control_info_t* find_remote_control_by_id(uint16_t id);

  protected:
    NimBLEScan*                             ble_scan = nullptr;
    std::vector<ble_remote_control_info_t*> remote_controls;

    ble_receive_callback_t     receive_callback;
    ble_remote_sync_callback_t sync_done_callback;

    bool _sync = false;
};

BLERemoteControlReceiver::BLERemoteControlReceiver() {}

BLERemoteControlReceiver::~BLERemoteControlReceiver() {}

void BLERemoteControlReceiver::begin() {
    NimBLEDevice::setScanDuplicateCacheSize(10);
    NimBLEDevice::init("");

    ble_scan = NimBLEDevice::getScan();
    ble_scan->setAdvertisedDeviceCallbacks(this, false);
    ble_scan->setActiveScan(false);
    ble_scan->setInterval(20);
    ble_scan->setWindow(10);
    ble_scan->setMaxResults(0);
    ble_scan->start(0, nullptr, false);
}

void BLERemoteControlReceiver::add_remote_control(ble_remote_control_info_t* remote_control) {
    if (remote_control->aes_key.size() != 16) {
        log_e("aes keylength is not equal 16!");
        return;
    }
    remote_controls.push_back(remote_control);
}

void BLERemoteControlReceiver::remove_remote_control(ble_remote_control_info_t* remote_control) {
    if (remote_control == nullptr) return;
    remote_controls.erase(std::remove(remote_controls.begin(), remote_controls.end(), remote_control), remote_controls.end());
}

void BLERemoteControlReceiver::on_command(ble_receive_callback_t callback) {
    receive_callback = callback;
}

void BLERemoteControlReceiver::sync_remotecontrol() {
    _sync = true;
}

void BLERemoteControlReceiver::on_sync(ble_remote_sync_callback_t callback) {
    sync_done_callback = callback;
}

void BLERemoteControlReceiver::onResult(NimBLEAdvertisedDevice* advertised_device) {
    std::string raw_data = advertised_device->getManufacturerData();

    if (raw_data.length() != sizeof(BLEManufacturerData)) return;
    BLEManufacturerData* manufacturer_data = (BLEManufacturerData*)raw_data.data();

    if (manufacturer_data->company_id != 0xFFFF) return;

    auto remote_id = manufacturer_data->remote_id;

    ble_remote_control_info_t* remote_control = find_remote_control_by_id(remote_id);

    if (remote_control == nullptr) {
        ble_scan->clearResults();
        return;
    }

    BLERemoteControlMessage message;

    decrypt_message(&message, manufacturer_data->encrypted_message, remote_control->aes_key.data());

    if (message.remote_id != manufacturer_data->remote_id) {
        ble_scan->clearResults();
        return;
    }

    if (_sync) {
        _sync                        = false;
        remote_control->rolling_code = message.rolling_code - 1;
        if (sync_done_callback) sync_done_callback(remote_control);
        return;
    }

    if (message.rolling_code > remote_control->rolling_code) {
        remote_control->rolling_code = message.rolling_code;

        ble_remote_command_info_t command_info;
        command_info.command        = message.command;
        command_info.remote_control = remote_control;

        if (receive_callback) receive_callback(&command_info);
    }

    ble_scan->clearResults();
};

ble_remote_control_info_t* BLERemoteControlReceiver::find_remote_control_by_id(uint16_t id) {
    for (auto remote_control : remote_controls) {
        if (remote_control->id == id) return remote_control;
    }
    return nullptr;
}