#pragma once

#include <stdint.h>

#include <vector>

using aes_key_t = std::vector<uint8_t>;

typedef struct ble_remote_control_info_t {
  public:
    ble_remote_control_info_t(uint16_t id, aes_key_t aes_key);
    ble_remote_control_info_t(uint16_t id, aes_key_t aes_key, uint32_t rolling_code);
    uint16_t  id;
    aes_key_t aes_key;
    uint32_t  rolling_code;
};

ble_remote_control_info_t::ble_remote_control_info_t(uint16_t id, aes_key_t aes_key)
    : id(id)
    , aes_key(aes_key) {
    if (rolling_code == 0) rolling_code = 1;
}

ble_remote_control_info_t::ble_remote_control_info_t(uint16_t id, aes_key_t aes_key, uint32_t rolling_code)
    : id(id)
    , aes_key(aes_key)
    , rolling_code(rolling_code) {
    if (rolling_code == 0) this->rolling_code = 1;
}
