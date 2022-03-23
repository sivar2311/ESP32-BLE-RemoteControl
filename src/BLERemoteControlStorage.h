#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "BLERemoteControlInfo.h"

struct ble_remote_control_storage_info_t {
    ble_remote_control_info_t* remote_control;

    bool delete_on_remove = false;
};

class BLERemoteControlStorage {
  public:
    BLERemoteControlStorage();
    virtual ~BLERemoteControlStorage();

    void add(ble_remote_control_info_t* remote_control, bool delete_on_remove = false);
    void add(ble_remote_control_info_t& remote_control);

    void remove(ble_remote_control_info_t* remote_control);
    void forEach(std::function<void(ble_remote_control_info_t*)> callback);
    ble_remote_control_info_t* operator[](size_t index);

    size_t                     size();
    ble_remote_control_info_t* find(uint16_t id);

  protected:
    ble_remote_control_storage_info_t* find(ble_remote_control_info_t* remote_control);

  protected:
    std::vector<ble_remote_control_storage_info_t*> remote_controls;
};

BLERemoteControlStorage::BLERemoteControlStorage() {}

BLERemoteControlStorage::~BLERemoteControlStorage() {
    for (auto element : remote_controls) {
        if (element->delete_on_remove) delete element->remote_control;
        delete element;
    }
    remote_controls.clear();
}

void BLERemoteControlStorage::add(ble_remote_control_info_t* remote_control, bool delete_on_remove) {
    ble_remote_control_storage_info_t* storage_info = new ble_remote_control_storage_info_t;

    storage_info->delete_on_remove = delete_on_remove;
    storage_info->remote_control   = remote_control;

    remote_controls.push_back(storage_info);
}

void BLERemoteControlStorage::add(ble_remote_control_info_t& remote_control) {
    add(&remote_control, false);
}

void BLERemoteControlStorage::remove(ble_remote_control_info_t* remote_control) {
    if (remote_control == nullptr) return;
    ble_remote_control_storage_info_t* storage_info = find(remote_control);
    if (storage_info == nullptr) return;
    if (storage_info->delete_on_remove) delete storage_info->remote_control;
    remote_controls.erase(std::remove(remote_controls.begin(), remote_controls.end(), storage_info), remote_controls.end());
    delete storage_info;
}

void BLERemoteControlStorage::forEach(std::function<void(ble_remote_control_info_t*)> callback) {
    if (callback == nullptr) return;
    for (auto& storage_info : remote_controls) {
        callback(storage_info->remote_control);
    }
}

ble_remote_control_info_t* BLERemoteControlStorage::find(uint16_t id) {
    for (auto storage_info : remote_controls) {
        if (storage_info->remote_control->id == id) return storage_info->remote_control;
    }
    return nullptr;
}

size_t BLERemoteControlStorage::size() {
    return remote_controls.size();
}

ble_remote_control_storage_info_t* BLERemoteControlStorage::find(ble_remote_control_info_t* remote_control) {
    for (auto& element : remote_controls) {
        if (element->remote_control == remote_control) return element;
    }
    return nullptr;
}

ble_remote_control_info_t* BLERemoteControlStorage::operator[](size_t index) {
    return remote_controls[index]->remote_control;
}