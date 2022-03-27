#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "BLERemoteControlInfo.h"

class BLERemoteControlStorage {
  public:
    virtual void                       save(const ble_remote_control_info_t* remote_control){};
    virtual ble_remote_control_info_t* load(uint16_t id, ble_remote_control_info_t* remote_control) { return nullptr; };
    virtual bool                       exist(uint16_t id) { return false; }
    virtual void                       remove(uint16_t id) {}
};
