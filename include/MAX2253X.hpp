#pragma once

#include <SPI.h>
#include <stdint.h>

struct MAX2253X {
  uint16_t CS_PIN;
  float VREF = 1.80;

  enum IDs {
    PROD_ID = 0x00,
    ADC1 = 0x01,
    ADC2 = 0x02,
    ADC3 = 0x03,
    ADC4 = 0x04,
    FADC1 = 0x05,
    FADC2 = 0x06,
    FADC3 = 0x07,
    FADC4 = 0x08,
    COUTHI1 = 0x09,
    COUTHI2 = 0x0a,
    COUTHI3 = 0x0b,
    COUTHI4 = 0x0c,
    COUTLO1 = 0x0d,
    COUTLO2 = 0x0e,
    COUTLO3 = 0x0f,
    COUTLO4 = 0x10,
    COUT_STATUS = 0x11,
    INTERRUPT_STATUS = 0x12,
    INTERRUPT_ENABLE = 0x13,
    CONTROL = 0x14,
    MAX22530_ID = 0x81,
  };

  void init(uint16_t pin) {
    CS_PIN = pin;
    pinMode(CS_PIN, OUTPUT);
    SPI.begin();
  };

  uint16_t read_register(IDs regAddress) {
    digitalWrite(CS_PIN, LOW);              // Select the device
    SPI.transfer((regAddress << 2) | 0x00); // A[5:0], W/ = 0, BURST = 0
    uint8_t msb = SPI.transfer(0x00);       // Receive MSB of data
    uint8_t lsb = SPI.transfer(0x00);       // Receive LSB of data
    digitalWrite(CS_PIN, HIGH);             // Deselect the device
    return uint16_t((msb << 8) | lsb);
  };

  float Convert_to_Voltage(IDs regAddress) {
    uint16_t result = 0;
    float voltage_result = 0.0;
    result = read_register(regAddress);
    if (regAddress == ADC1 || regAddress == ADC2 || regAddress == ADC3 ||
        regAddress == ADC4) {
      // 16th bit of ADCx set to 1 meaning ADC not updated since last read
      if (result > 4096) {
        return 100.00;
      }
      voltage_result = (float)VREF * result / 4096;
    } else if (regAddress == COUTHI1 || regAddress == COUTHI2 ||
               regAddress == COUTHI3 || regAddress == COUTHI4) {
      result = result & 0x0FFF;
      voltage_result = (float)VREF * result / 4096;
    } else {
      voltage_result = (float)VREF * result / 4096;
    }
    return voltage_result;
  }
};
