#pragma once

#include <Arduino.h>

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
typedef struct device_status_t {
  const bool project_on_main_or_master = FW_PROJECT_IS_MAIN_OR_MASTER;
  const bool project_is_dirty = FW_PROJECT_IS_DIRTY;
  const uint32_t firmware_version = AUTO_VERSION;
  uint16_t on_time_seconds = 0;
  uint8_t temp : 7 = 0;
  uint8_t humidity : 7 = 0;
} device_status_t;

void set_temp(device_status_t *s, uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    s->temp = value;
  } else {
    Serial.println("Value out of range for 7 bits (0-127).");
  }
}

void set_humidity(device_status_t *s, uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    s->humidity = value;
  } else {
    Serial.println("Value out of range for 7 bits (0-127).");
  }
}
