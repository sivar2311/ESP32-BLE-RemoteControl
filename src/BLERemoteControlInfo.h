#pragma once

#include <stdint.h>

#include <vector>

using aes_key_t = uint8_t[16];

struct ble_remote_control_info_t {
  public:
    ble_remote_control_info_t(uint16_t id, std::initializer_list<uint8_t> aes_key);
    ble_remote_control_info_t(uint16_t id, std::initializer_list<uint8_t> aes_key, uint32_t rolling_code);
    void init_aes_key_from_initializer_list(std::initializer_list<uint8_t>& src);
    uint16_t  id;
    aes_key_t aes_key;
    uint32_t  rolling_code;
};

ble_remote_control_info_t::ble_remote_control_info_t(uint16_t id, std::initializer_list<uint8_t> aes_key)
    : id(id)
    , aes_key{0} {
    if (rolling_code == 0) rolling_code = 1;
    init_aes_key_from_initializer_list(aes_key);
}

ble_remote_control_info_t::ble_remote_control_info_t(uint16_t id, std::initializer_list<uint8_t> aes_key, uint32_t rolling_code)
    : id(id)
    , aes_key{0}
    , rolling_code(rolling_code) {
    if (this->rolling_code == 0) this->rolling_code = 1;
    init_aes_key_from_initializer_list(aes_key);
}

void ble_remote_control_info_t::init_aes_key_from_initializer_list(std::initializer_list<uint8_t>& src) {
    if (src.size() == sizeof(aes_key_t)) {
        size_t i = 0;
        for (auto element : src) {
            this->aes_key[i++] = element;
        }
    }
}
