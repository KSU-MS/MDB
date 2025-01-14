#pragma once

#include <Arduino.h>

// These values are provided by the python script ran by the lib_dep
// https://github.com/KSU-MS/pio-git-hash-gen
#ifndef AUTO_VERSION
#warning "AUTO_VERSION was not defined by the generator!"
#define AUTO_VERSION 0xdeadbeef
#endif

#ifndef FW_PROJECT_IS_DIRTY
#warning "FW_PROJECT_IS_DIRTY was not defined by the generator!"
#define FW_PROJECT_IS_DIRTY 1
#endif

#ifndef FW_PROJECT_IS_MAIN_OR_MASTER
#warning "FW_PROJECT_IS_MAIN_OR_MASTER was not defined by the generator!"
#define FW_PROJECT_IS_MAIN_OR_MASTER 0
#endif

// ideally little endian bc teensy
union {
  struct {
    bool project_on_main_or_master = FW_PROJECT_IS_MAIN_OR_MASTER;
    bool project_is_dirty = FW_PROJECT_IS_DIRTY;
    uint32_t firmware_version = AUTO_VERSION;
    uint16_t on_time_seconds = 0;
    uint8_t humidity : 7;
    uint8_t temp : 7;
  };

  byte b[8];
} status_message;

void set_temp(uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    status_message.temp = value;
  } else {
    Serial.println("Value out of range for 7 bits (0-127).");
  }
}

void set_humidity(uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    status_message.humidity = value;
  } else {
    Serial.println("Value out of range for 7 bits (0-127).");
  }
}
