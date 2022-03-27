#pragma once

#include <Preferences.h>

#include "BLERemoteControlStorage.h"

class NVSRemoteControlStorage : public BLERemoteControlStorage {
  public:
    void begin(const char* name, const char* partition_label = nullptr);
    void clear();
    void end();

    void                       save(const ble_remote_control_info_t* remote_control) override;
    ble_remote_control_info_t* load(uint16_t id, ble_remote_control_info_t* remote_control) override;
    bool                       exist(uint16_t id) override;
    void                       remove(uint16_t id) override;

  protected:
    typedef char nvs_key_t[5];
    const char*  make_key(const uint16_t id, nvs_key_t key);

  protected:
    Preferences preferences;
};

void NVSRemoteControlStorage::begin(const char* name, const char* partition_label) {
    preferences.begin(name, false, partition_label);
}

void NVSRemoteControlStorage::clear() {
    preferences.clear();
}

void NVSRemoteControlStorage::end() {
    preferences.end();
}

void NVSRemoteControlStorage::save(const ble_remote_control_info_t* remote_control) {
    nvs_key_t key;
    preferences.putBytes(make_key(remote_control->id, key), remote_control, sizeof(ble_remote_control_info_t));
}

ble_remote_control_info_t* NVSRemoteControlStorage::load(uint16_t id, ble_remote_control_info_t* remote_control) {
    nvs_key_t key;
    if (!exist(id)) return nullptr;

    size_t    size = preferences.getBytes(make_key(id, key), remote_control, sizeof(ble_remote_control_info_t));
    if (size == 0) return nullptr;

    return remote_control;
}

bool NVSRemoteControlStorage::exist(uint16_t id) {
    nvs_key_t key;
    return preferences.isKey(make_key(id, key));
}

void NVSRemoteControlStorage::remove(uint16_t id) {
    nvs_key_t key;
    preferences.remove(make_key(id, key));
}

const char* NVSRemoteControlStorage::make_key(const uint16_t id, nvs_key_t key) {
    snprintf(key, sizeof(nvs_key_t), "%04x", id);
    return key;
}