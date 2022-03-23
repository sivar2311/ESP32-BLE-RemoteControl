#pragma once

#include <Preferences.h>

#include "BLERemoteControlStorage.h"

const char* storage_key_name_size = "size";

class NVSRemoteControlStorage : public BLERemoteControlStorage, protected Preferences {
  public:
    using Preferences::begin;
    using Preferences::clear;
    using Preferences::end;

    bool load();
    void save();

    void remove(ble_remote_control_info_t* remote_control);
};

bool NVSRemoteControlStorage::load() {
    uint16_t size = getUShort(storage_key_name_size, 0);
    if (size == 0) return false;

    for (size_t i = 0; i < size; i++) {
        ble_remote_control_info_t* rc = new ble_remote_control_info_t(i, false);
        char                       key[5];
        sprintf(key, "%4x", i);
        getBytes((const char*)key, rc, sizeof(ble_remote_control_info_t));
        add(rc, true);
    }

    return true;
}

void NVSRemoteControlStorage::save() {
    putUShort(storage_key_name_size, remote_controls.size());

    for (size_t i = 0; i < remote_controls.size(); i++) {
        char key[5];
        sprintf(key, "%4x", i);
        putBytes((const char*)key, remote_controls[i]->remote_control, sizeof(ble_remote_control_info_t));
    }
}

void NVSRemoteControlStorage::remove(ble_remote_control_info_t* remote_control) {
    BLERemoteControlStorage::remove(remote_control);
    Preferences::clear();
    save();
}