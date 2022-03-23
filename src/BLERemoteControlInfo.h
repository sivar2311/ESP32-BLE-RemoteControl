#pragma once

#include <stdint.h>

#include <vector>

using aes_key_t = uint8_t[16];

class __attribute__((packed)) ble_remote_control_info_t {
  public:
    ble_remote_control_info_t();
    ble_remote_control_info_t(const ble_remote_control_info_t& other);
    ble_remote_control_info_t(uint16_t id);
    ble_remote_control_info_t(uint16_t id, bool new_key);
    ble_remote_control_info_t(uint16_t id, std::initializer_list<uint8_t> aes_key, uint32_t rolling_code = 1);

  public:
    uint16_t  id;
    aes_key_t aes_key;
    uint32_t  rolling_code;

  protected:
    void init_aes_key_from_initializer_list(std::initializer_list<uint8_t>& src);
};

ble_remote_control_info_t::ble_remote_control_info_t() : id(0), aes_key{0}, rolling_code(0) {}

ble_remote_control_info_t::ble_remote_control_info_t(const ble_remote_control_info_t& other) {
  this->id = other.id;
  this->rolling_code = other.rolling_code;
  memcpy(this->aes_key, other.aes_key, sizeof(this->aes_key));
}

ble_remote_control_info_t::ble_remote_control_info_t(uint16_t id) : id(id), aes_key{0}, rolling_code(1) {}

ble_remote_control_info_t::ble_remote_control_info_t(uint16_t id, bool new_key) : id(id), rolling_code(1) {
  if (new_key) {
    for (size_t i=0; i < sizeof(aes_key_t); i++) {
      aes_key[i] = random(0xFF);
    }
  }
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
