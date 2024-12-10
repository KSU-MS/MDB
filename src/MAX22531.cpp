#include "MAX22531.hpp"

uint16_t MAX2253X::read_register(uint8_t regAddress) {
  uint16_t result = 0;
  uint8_t crc_read = 0;
  uint32_t frame = (uint32_t)(regAddress << 2);

  // Read/ Write bit set to 0 and burst bit set to 0.(b'18 is 0 and b'17 is 0)
  if (!crc_enable) {
    digitalWrite(CS_PIN, LOW);
    SPI.transfer(frame);
    result = SPI.transfer16(0);
    digitalWrite(CS_PIN, HIGH);
  } else {
    uint8_t crc = crc_compute_2(frame << 16);
    digitalWrite(CS_PIN, LOW);
    SPI.transfer(frame);
    result = SPI.transfer16(0);
    crc_read = SPI.transfer(crc);
    digitalWrite(CS_PIN, HIGH);
    crc = crc_compute_2((frame << 16) + (result));
    if (crc != crc_read) {
      printf("CRC Read from MAX22530 is incorrect");
      return (0x10000);
    } else
      printf("CRC Matched");
  }
  return result;
}

// Writes to any register.
// regAddress - The address of the register to read.
// regvalue - The register value to write to the register.
void MAX2253X::write_register(uint8_t regAddress, uint16_t regValue) {
  // Register_address address = COUTH1, data = 8B32h or 35634d
  uint32_t data_frame1 = 0x0000;
  uint8_t crc_code = 0;

  /// Read/ Write bit set to 1, burst bit set to 0.
  if ((!crc_enable)) {
    data_frame1 = (uint32_t)((regAddress << 2) + (1 << 1));
  }

  /// crc enabled, construct frame + compute crc.
  else {
    data_frame1 = (uint32_t)((regAddress << 2) + (1 << 1));
    crc_code = crc_compute_2((data_frame1 << 16) + (regValue << 0));
  }

  digitalWrite(CS_PIN, LOW);
  SPI.transfer(data_frame1);
  SPI.transfer16(regValue);
  if (crc_enable) {
    SPI.transfer(crc_code);
  }
  digitalWrite(CS_PIN, HIGH);
}

// Function to perform burst read on ADC1 or FADC1 registers only.
// regAddress - The address of the register to burst read. (ADC1 or FADC1)

// return - Global Burst read variables Burst_reg0, Burst_reg1, Burst_reg2,
// Burst_reg3, Burst_INT_status are updated 0 - Burst read is not performed and
// no action is taken 0x10000 – CRC mismatch occured void
// Only Two Options for Burst read ADC1 & FADC1 Only Two Options for Burst read
// ADC1 & FADC1. Register_address address = FADC1
void MAX2253X::Burst_read_register(uint8_t regAddress) {
  if (regAddress == ADC1 || regAddress == FADC1) {
    uint8_t crc, crc_read = 0;
    uint32_t frame = (uint32_t)((regAddress << 2) + (1 << 0));
    static uint32_t result[5];
    if (!crc_enable) // Read/ Write bit set to 0 and burst bit set to 1. 18th
                     // bit=0 and 17th bit= 1
    {
      digitalWrite(CS_PIN, LOW);
      SPI.transfer(frame);
      Burst_reg1 = SPI.transfer16(0);
      Burst_reg2 = SPI.transfer16(0);
      Burst_reg3 = SPI.transfer16(0);
      Burst_reg4 = SPI.transfer16(0);
      Burst_INT_status = SPI.transfer16(0);
      digitalWrite(CS_PIN, HIGH);
    } else if (crc_enable) {
      digitalWrite(CS_PIN, LOW);
      SPI.transfer(frame);
      result[0] = SPI.transfer16(0);
      result[1] = SPI.transfer16(0);
      result[2] = SPI.transfer16(0);
      result[3] = SPI.transfer16(0);
      result[4] = SPI.transfer16(0);
      crc_read = SPI.transfer(0);
      digitalWrite(CS_PIN, HIGH);
      crc = crc_compute_burst(((frame << 16) + (result[0] << 0)),
                              ((result[1] << 16) + (result[2] << 0)),
                              ((result[3] << 16) + (result[4] << 0)));
      if (crc != crc_read) {
        printf("CRC Read from MAX22530 is incorrect\n");
      } else {
        printf("CRC Matched \n");
      }
      Burst_reg1 = result[0];
      Burst_reg2 = result[1];
      Burst_reg3 = result[2];
      Burst_reg4 = result[3];
      Burst_INT_status = result[4];
    }
  } else {
  }
}

// Initializes the MAX22530 and checks if the device is present for first time.
// If the device is present, write CONTROL Register b'[15] = 0 to disable CRC.

// return Result of the initialization procedure. Example : 1 - if
// initialization was successful(ID is 0x0B).0 -if initialization was
// unsuccessful.

uint8_t MAX2253X::Init(uint16_t pin) {
  CS_PIN = pin;

  uint8_t status = 0x1;
  if ((MAX2253X::read_register(PROD_ID) != MAX22530_ID)) {
    status = 0x0;
  }
  uint16_t temp = MAX2253X::read_register(CONTROL);
  if (temp > 32767) {
    crc_enable = true;
    DIS_CRC();
  }
  return (status);
}

// Write bit field 0 of Control register in order to reset the part.
void MAX2253X::Reset(void) {
  write_register(CONTROL, 0);
  crc_enable = false;
}

// Write bit field 1 of Control register in order to soft reset the part.
void MAX2253X::softReset(void) { write_register(CONTROL, 2); }

// Toggles bit in a given register. Used in conjunction with INT_EN and CONTROL
// to toggle a bit. n = 0 for First bit position and so on.
// regAddress - The address of the register to read.
// bit_position - The position of the register to toggle.
void MAX2253X::Register_bit_toggle(uint8_t regAddress, uint8_t bit_position) {
  if (regAddress == 0x14 && bit_position == 0) {
    printf("Use hardware Reset function");
  } else {
    int n = bit_position;
    uint16_t current_data = read_register(regAddress);
    uint16_t new_data = current_data ^ (1UL << n);
    write_register(regAddress, new_data);
  }
}

// Enables CRC checks, use this on init
void MAX2253X::EN_CRC() {
  uint16_t register_value = read_register(CONTROL);
  /*set EN CRC bit in Control Register*/
  if (register_value < 32768) {
    register_value |= 32768; // if register value = 0x8000
    write_register(CONTROL, register_value);
    crc_enable = true;
  }
}

// Disables CRC checks, use this on init
void MAX2253X::DIS_CRC() {
  uint16_t register_value = read_register(CONTROL);
  // disable CRC bit in Control Register
  register_value &= 32767; // if register value = 0x7FFF
  write_register(CONTROL, register_value);
  crc_enable = false;
}

// Enables End of conversion Hardware pin @ MAX2253x INT.
void MAX2253X::EN_EOC() {
  uint16_t register_value = read_register(INTERRUPT_ENABLE);
  // disable CRC bit in Control Register
  register_value |= 4096; // if register value = 0x1000
  write_register(INTERRUPT_ENABLE, register_value);
}

// Disbles End of conversion Hardware pin @ MAX2253x INT.
void MAX2253X::DIS_EOC() {
  uint16_t register_value = read_register(INTERRUPT_ENABLE);
  // disable CRC bit in Control Register
  register_value &= 4095; // if register value = 0x0FFF
  write_register(INTERRUPT_ENABLE, register_value);
}

// returns voltage value of content in any of the ADCX, FADCX, COUTX registers
// regAddress - The address of the register to read.
// return - voltage from int value of reg data 100 if not updated from last read
float MAX2253X::Convert_to_Voltage(uint8_t regAddress) {
  uint16_t result = 0;
  float voltage_result = 0.0;
  result = read_register(regAddress);
  if (regAddress == ADC1 || regAddress == ADC2 || regAddress == ADC3 ||
      regAddress == ADC4) {
    // 16th bit of ADCx set to 1 meaning no new value since last read operation
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

//
////
////// CRC functions
uint8_t MAX2253X::crc_compute_2(uint32_t frame) {
  uint8_t frame_high = (uint8_t)((frame & 0x00FF0000) >> 16);
  uint8_t frame_mid = (uint8_t)((frame & 0x0000FF00) >> 8);
  uint8_t frame_low = (uint8_t)(frame & 0x000000FF);
  uint8_t message[] = {frame_high, frame_mid, frame_low, 0x00};
  int i, j;
  message[3] = getCRC(message, 3);
  return (message[3]);
}

// Used with Register Burst Read function
uint8_t MAX2253X::crc_compute_burst(uint32_t frame1, uint32_t frame2,
                                    uint32_t frame3) {

  uint8_t frame1_high = (uint8_t)((frame1 & 0x00FF0000) >> 16);
  uint8_t frame1_mid = (uint8_t)((frame1 & 0x0000FF00) >> 8);
  uint8_t frame1_low = (uint8_t)(frame1 & 0x000000FF);
  uint8_t frame2_high = (uint8_t)((frame2 & 0xFF000000) >> 24);
  uint8_t frame2_mid1 = (uint8_t)((frame2 & 0x00FF0000) >> 16);
  uint8_t frame2_mid2 = (uint8_t)((frame2 & 0x0000FF00) >> 8);
  uint8_t frame2_low = (uint8_t)(frame2 & 0x000000FF);
  uint8_t frame3_high = (uint8_t)((frame3 & 0xFF000000) >> 24);
  uint8_t frame3_mid1 = (uint8_t)((frame3 & 0x00FF0000) >> 16);
  uint8_t frame3_mid2 = (uint8_t)((frame3 & 0x0000FF00) >> 8);
  uint8_t frame3_low = (uint8_t)(frame3 & 0x000000FF);

  uint8_t crc_calculated = 0x00;

  uint8_t message[] = {frame1_high, frame1_mid,  frame1_low, frame2_high,
                       frame2_mid1, frame2_mid2, frame2_low, frame3_high,
                       frame3_mid1, frame3_mid2, frame3_low, 0x00};
  int i, j;
  crc_calculated = getCRC(message, 11);
  return crc_calculated;
}

uint8_t MAX2253X::getCRC(uint8_t message[], unsigned int length) {
  uint8_t i, crc = 0;

  for (i = 0; i < length; i++)
    crc = CRCTable[crc ^ message[i]];
  return crc;
}
