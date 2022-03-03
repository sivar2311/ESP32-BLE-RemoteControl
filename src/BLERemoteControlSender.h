#pragma once

#include <NimBLEDevice.h>

#include "BLERemoteControlBase.h"

class BLERemoteControlSender : public BLERemoteControlBaseClass {
  public:
    void begin();

    void send_command(uint16_t command, ble_remote_control_info_t* remote_control);

  protected:
    NimBLEAdvertising* ble_advertising = nullptr;
};

void BLERemoteControlSender::begin() {
    NimBLEDevice::init("");
    ble_advertising = NimBLEDevice::getAdvertising();
}

void BLERemoteControlSender::send_command(uint16_t command, ble_remote_control_info_t* remote_control) {
    if (ble_advertising == false) return;

    BLEManufacturerData man_data;
    man_data.company_id = 0xFFFF;
    man_data.remote_id  = remote_control->id;

    BLERemoteControlMessage message;
    message.remote_id    = remote_control->id;
    message.command      = command;
    message.rolling_code = remote_control->rolling_code++;
    message.nonce        = random(0xFFFF);

    encrypt_message(man_data.encrypted_message, &message, remote_control->aes_key.data());

    ble_advertising->stop();
    ble_advertising->setManufacturerData(std::string((const char*)&man_data, sizeof(BLEManufacturerData)));
    ble_advertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);
    ble_advertising->start(1);
}