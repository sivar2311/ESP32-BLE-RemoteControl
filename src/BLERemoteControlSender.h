#pragma once

#include <NimBLEDevice.h>

#include "BLERemoteControlBase.h"

class BLERemoteControlSender : public BLERemoteControlBaseClass {
  public:
    void begin(uint32_t pin = 123456);

    void send_command(uint32_t command, ble_remote_control_info_t* remote_control);
    void start_sync(ble_remote_control_info_t& remote_control);

  protected:
    NimBLEAdvertising* ble_advertising = nullptr;

    NimBLEAdvertisementData sync_adv_data;

    NimBLEServer*         ble_server         = nullptr;
    NimBLEService*        ble_service        = nullptr;
    NimBLECharacteristic* ble_characteristic = nullptr;
};

void BLERemoteControlSender::begin(uint32_t pin) {
    NimBLEDevice::init("");

    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityPasskey(pin);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

    ble_server = NimBLEDevice::createServer();
    ble_server->advertiseOnDisconnect(false);

    ble_service        = ble_server->createService(NimBLEUUID(remote_control_service_uuid));
    ble_characteristic = ble_service->createCharacteristic(
        NimBLEUUID(remote_control_characteristic_uuid),
        NIMBLE_PROPERTY::READ |
        NIMBLE_PROPERTY::READ_ENC |
        NIMBLE_PROPERTY::READ_AUTHEN 
    );
    ble_service->start();

    ble_advertising = NimBLEDevice::getAdvertising();
    ble_advertising->setMinInterval(0x20);
    ble_advertising->setMaxInterval(0xA0);

    sync_adv_data.setFlags(0x06);
    sync_adv_data.setCompleteServices(NimBLEUUID(remote_control_service_uuid));
}

void BLERemoteControlSender::start_sync(ble_remote_control_info_t& remote_control) {
    ble_characteristic->setValue(remote_control);
    remote_control.rolling_code++;

    ble_advertising->setAdvertisementType(BLE_GAP_CONN_MODE_UND);
    ble_advertising->setAdvertisementData(sync_adv_data);
    ble_advertising->start(5);
}

void BLERemoteControlSender::send_command(uint32_t command, ble_remote_control_info_t* remote_control) {
    BLEManufacturerData man_data;
    man_data.company_id = 0xFFFF;
    man_data.remote_id  = remote_control->id;

    BLERemoteControlMessage message;
    message.remote_id    = remote_control->id;
    message.command      = command;
    message.rolling_code = remote_control->rolling_code++;
    message.nonce        = random(0xFFFF);

    encrypt_message(man_data.encrypted_message, &message, remote_control->aes_key);

    NimBLEAdvertisementData adv_data;
 
    adv_data.setFlags(0x00);
    adv_data.setManufacturerData(std::string((const char*)&man_data, sizeof(BLEManufacturerData)));
 
    ble_advertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);
    ble_advertising->setAdvertisementData(adv_data);

    ble_advertising->start(1);
}