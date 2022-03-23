#pragma once

#include <Arduino.h>
#include <mbedtls/aes.h>
#include <stdint.h>

#include <algorithm>
#include <vector>

#include "BLERemoteControlInfo.h"

const char* remote_control_service_uuid        = "6b275c4b-adfc-457f-a89a-441ce79a3921";
const char* remote_control_characteristic_uuid = "fd8554ed-2518-4ccf-8709-e13be9bb614b";

struct __attribute__((packed)) BLERemoteControlMessage {
    uint16_t remote_id;
    uint32_t command;
    uint32_t rolling_code;
    uint16_t nonce;
    uint8_t  reserved[4];
};

struct __attribute__((packed)) BLEManufacturerData {
    uint16_t company_id;
    uint16_t remote_id;
    uint8_t  encrypted_message[16];
};

class BLERemoteControlBaseClass {
  public:
    BLERemoteControlBaseClass();
    ~BLERemoteControlBaseClass();

  protected:
    void encrypt_message(uint8_t* encoded, BLERemoteControlMessage* message, const uint8_t* key);
    void decrypt_message(BLERemoteControlMessage* message, uint8_t* encoded, const uint8_t* key);
};

BLERemoteControlBaseClass::BLERemoteControlBaseClass() {}

BLERemoteControlBaseClass::~BLERemoteControlBaseClass() {}

void BLERemoteControlBaseClass::encrypt_message(uint8_t* encoded, BLERemoteControlMessage* message, const uint8_t* key) {
    uint8_t plain[16];
    memcpy(plain, message, sizeof(BLERemoteControlMessage));

    esp_aes_context ctx;
    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, (const unsigned char*)key, 16 * 8);
    esp_aes_crypt_ecb(&ctx, ESP_AES_ENCRYPT, (unsigned char*)plain, (unsigned char*)encoded);
    esp_aes_free(&ctx);
}

void BLERemoteControlBaseClass::decrypt_message(BLERemoteControlMessage* message, uint8_t* encoded, const uint8_t* key) {
    uint8_t         plain[16];
    esp_aes_context ctx;
    esp_aes_init(&ctx);
    esp_aes_setkey(&ctx, (const unsigned char*)key, 16 * 8);
    esp_aes_crypt_ecb(&ctx, ESP_AES_DECRYPT, (unsigned char*)encoded, (unsigned char*)plain);
    esp_aes_free(&ctx);

    memcpy(message, plain, sizeof(BLERemoteControlMessage));
}