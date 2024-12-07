#include "MAX22531.hpp"

uint16_t MAX2253X::read_register(uint8_t regAddress) {
  uint16_t result = 0;
  uint8_t crc_read = 0;
  uint32_t frame = (uint32_t)(regAddress << 2);

  // Read/ Write bit set to 0 and burst bit set to 0.(b'18 is 0 and b'17 is 0)
  if (!crc_enable) {
    CS_LOW;
    SPI.transfer8(frame);
    result = SPI.transfer16(0);
    CS_HIGH;
  } else {
    uint8_t crc = crc_compute_2(frame << 16);
    CS_LOW;
    SPI.transfer8(frame);
    result = SPI.transfer16(0);
    crc_read = SPI.transfer8(crc);
    CS_HIGH;
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
uint16_t MAX2253X::write_register(uint8_t regAddress, uint16_t regValue) {
  // Register_address address = COUTH1, data = 8B32h or 35634d
  uint32_t data_frame1 = 0x0000;
  uint8_t crc_code = 0;

  if ((!crc_enable)) /// Read/ Write bit set to 1, burst bit set to 0.
  {
    data_frame1 = (uint32_t)((regAddress << 2) + (1 << 1));
  } else /// crc enabled, construct frame + compute crc.
  {
    data_frame1 = (uint32_t)((regAddress << 2) + (1 << 1));
    crc_code = crc_compute_2((data_frame1 << 16) + (regValue << 0));
  }

  CS_LOW;
  SPI.transfer8(data_frame1);
  SPI.transfer16(regValue);
  if (crc_enable) {
    SPI.transfer8(crc_code);
  }
  CS_HIGH;
}

// Function to perform burst read on ADC1 or FADC1 registers only.
// regAddress - The address of the register to burst read. (ADC1 or FADC1)

// return - Global Burst read variables Burst_reg0, Burst_reg1, Burst_reg2,
// Burst_reg3, Burst_INT_status are updated 0 - Burst read is not performed and
// no action is taken 0x10000 – CRC mismatch occured void
// MAX2253X::Burst_read_register(uint8_t regAddress) Only Two Options for Burst
// read ADC1 & FADC1 Only Two Options for Burst read ADC1 & FADC1.
// Register_address address = FADC1
{
  if (regAddress == ADC1 || regAddress == FADC1) {
    uint8_t crc, crc_read = 0;
    uint32_t frame = (uint32_t)((regAddress << 2) + (1 << 0));
    static uint32_t result[5];
    if (!crc_enable) // Read/ Write bit set to 0 and burst bit set to 1. 18th
                     // bit=0 and 17th bit= 1
    {
      CS_LOW;
      SPI.transfer8(frame);
      Burst_reg1 = SPI.transfer16(0);
      Burst_reg2 = SPI.transfer16(0);
      Burst_reg3 = SPI.transfer16(0);
      Burst_reg4 = SPI.transfer16(0);
      Burst_INT_status = SPI.transfer16(0);
      CS_HIGH;
    } else if (crc_enable) {
      CS_LOW;
      SPI.transfer8(frame);
      result[0] = SPI.transfer16(0);
      result[1] = SPI.transfer16(0);
      result[2] = SPI.transfer16(0);
      result[3] = SPI.transfer16(0);
      result[4] = SPI.transfer16(0);
      crc_read = SPI.transfer8(0);
      CS_HIGH;
      crc = crc_compute_burst(((frame << 16) + (result[0] << 0)),
                              ((result[1] << 16) + (result[2] << 0)),
                              ((result[3] << 16) + (result[4] << 0)));
      if (crc != crc_read) {
        printf("CRC Read from MAX22530 is incorrect\n");
        return (0x10000);
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
    return 0;
  }
}

// Initializes the MAX22530 and checks if the device is present for first time.
// If the device is present, write CONTROL Register b'[15] = 0 to disable CRC.

// return Result of the initialization procedure. Example : 1 - if
// initialization was successful(ID is 0x0B).0 -if initialization was
// unsuccessful.

uint8_t MAX2253X::Init(void) {
  uint8_t status = 0x1;
  if ((MAX2253X::read_register(PROD_ID) != MAX22530_ID)) {
    status = 0x0;
  }
  uint16_t temp = MAX2253X::read_register(CONTROL);
  if (temp > 32767) {
    crc_enable = true;
    MAX2253x_DIS_CRC();
  }
  return (status);
}

// Write bit field 0 of Control register in order to reset the part.
void MAX2253X::Reset(void) {
  MAX2253X::write_register(CONTROL, 0);
  crc_enable = false;
}

// Write bit field 1 of Control register in order to soft reset the part.
void MAX2253X::softReset(void) { MAX22530_write_register(CONTROL, 2); }

// Toggles bit in a given register. Used in conjunction with INT_EN and CONTROL
// to toggle a bit. n = 0 for First bit position and so on.
// regAddress - The address of the register to read.
// bit_position - The position of the register to toggle.
void MAX2253X::Register_bit_toggle(uint8_t regAddress, uint8_t bit_position) {
  if (regAddress == 0x14 && bit_position == 0) {
    printf("Use hardware Reset function");
    return (0);
  } else {
    int n = bit_position;
    uint16_t current_data = MAX2253X::read_register(regAddress);
    uint16_t new_data = current_data ^ (1UL << n);
    MAX2253X::write_register(regAddress, new_data);
  }
}

// Enables CRC checks, use this on init
void MAX2253X::EN_CRC() {
  uint16_t register_value = MAX2253X::read_register(CONTROL);
  /*set EN CRC bit in Control Register*/
  if (register_value < 32768) {
    register_value |= 32768; // if register value = 0x8000
    MAX2253X::write_register(CONTROL, register_value);
    crc_enable = true;
  }
}

// Disables CRC checks, use this on init
void MAX2253X::DIS_CRC() {
  uint16_t register_value = MAX2253X::read_register(CONTROL);
  // disable CRC bit in Control Register
  register_value &= 32767; // if register value = 0x7FFF
  MAX2253X::write_register(CONTROL, register_value);
  crc_enable = false;
}

// Enables End of conversion Hardware pin @ MAX2253x INT.
void MAX2253X::EN_EOC() {
  uint16_t register_value = MAX2253X::read_register(INTERRUPT_ENABLE);
  // disable CRC bit in Control Register
  register_value |= 4096; // if register value = 0x1000
  MAX2253X::write_register(INTERRUPT_ENABLE, register_value);
}

// Disbles End of conversion Hardware pin @ MAX2253x INT.
void MAX2253X::DIS_EOC() {
  uint16_t register_value = MAX2253X::read_register(INTERRUPT_ENABLE);
  // disable CRC bit in Control Register
  register_value &= 4095; // if register value = 0x0FFF
  MAX2253X::write_register(INTERRUPT_ENABLE, register_value);
}
