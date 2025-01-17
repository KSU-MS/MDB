#pragma once

#include "main.hpp"

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

byte board_status[8];

void set_time() {
  uint16_t fella = uint16_t((millis() / 1000));
#ifdef DEBUG
  Serial.print("Board on time: ");
  Serial.println(fella);
#endif

  board_status[2] = (fella >> 8) & 0xFF; // Store the high byte
  board_status[3] = fella & 0xFF;        // Store the low byte

  board_status[4] = (AUTO_VERSION >> 24) & 0xFF; // MSB (Most Significant Byte)
  board_status[5] = (AUTO_VERSION >> 16) & 0xFF; // Next byte
  board_status[6] = (AUTO_VERSION >> 8) & 0xFF;  // Next byte
  board_status[7] = AUTO_VERSION & 0xFF;         // LSB (Least Significant Byte)
}

void set_temp(uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    board_status[0] = (FW_PROJECT_IS_DIRTY << 7) | (value & 0x7F);

#ifdef DEBUG
    Serial.print("Board temp: ");
    Serial.println(value);
#endif
  } else {
    Serial.println("Value out of range for 7 bits (0-127).");
  }
}

void set_humidity(uint8_t value) {
  // Ensure the value is within the 7-bit range (0-127)
  if (value <= 127) {
    board_status[1] = (FW_PROJECT_IS_MAIN_OR_MASTER << 7) | (value & 0x7F);

#ifdef DEBUG
    Serial.print("Board humidity: ");
    Serial.println(value);
#endif
  } else {
    Serial.println("Value out of range for 7 bits (0-127).");
  }
}
