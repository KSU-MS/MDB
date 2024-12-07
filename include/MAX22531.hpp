#pragma once
//
////
////// MAX22530 Registers
#define PROD_ID 0x00
#define ADC1 0x01
#define ADC2 0x02
#define ADC3 0x03
#define ADC4 0x04
#define FADC1 0x05
#define FADC2 0x06
#define FADC3 0x07
#define FADC4 0x08
#define COUTHI1 0x09
#define COUTHI2 0x0a
#define COUTHI3 0x0b
#define COUTHI4 0x0c
#define COUTLO1 0x0d
#define COUTLO2 0x0e
#define COUTLO3 0x0f
#define COUTLO4 0x10
#define COUT_STATUS 0x11
#define INTERRUPT_STATUS 0x12
#define INTERRUPT_ENABLE 0x13
#define CONTROL 0x14

#define MAX22530_ID 0x81
#define VREF 1.80

struct MAX2253X {
  //
  ////
  ////// SPI and GPIO settings
  uint16_t CS_PIN; // defining the pin that controls the output

  // Fuck all this noise for now
  // #define CS_PIN_OUT " " // defining the pin Mode on uC side
  // #define CS_LOW " "     // digital Write of uC CS_PIN LOW
  // #define CS_HIGH " "    // digital Write of uC CS_PIN HIGH
  // #define GPIO1_PIN " " // Pin of the uC to which the ADC Hardware EOC pin
  // INTB Ready pin is connected for determining the end of conversion using a
  // polling sequence.

  // #define GPIO1_PIN_IN " " // Defining the pin mode of GPIO1_PIN

  // #define GPIO1_STATE " "  // Configuring the GPIO1_PIN as read
  // state

  // #define SPI_SETTINGS " " // Configure the SPI settings on uC side
  // for. ex: 8 MHz clock, MSB first, SPI CPOL 0, CPHA 0

  // MAX22530 HARDWARE INTERRUPT PIN-GPO
  // #define MAX22530_INTB_RDY_STATE // GPIO_STATE used by uC

  //
  ////
  ////// Function Defs
  // Starts comms and checks if the device is present
  uint8_t Init(uint16_t pin);

  // Power cycles the device
  void Reset(void);

  // Resets the device
  void softReset(void);

  // Reads a register
  uint16_t read_register(uint8_t regAddress);

  // Writes a value to a register
  uint16_t write_register(uint8_t regAddress, uint16_t regValue);

  // "Burst" reads a register, in short this means reading more register at a
  // time
  void Burst_read_register(uint8_t regAddress);

  // Enables the Hardware End of Conversion Interrupt Ready Bit to go LOW for
  // ADC
  void EN_EOC();

  // Disables the Hardware Interrupt Ready Bit to go LOW for ADC
  void DIS_EOC();

  // Enables CRC, or checksum bytes
  void EN_CRC();

  // Disables CRC, or checksum bytes
  void DIS_CRC();

  // Toggles a bit based on bit position to the given register.
  void Register_bit_toggle(uint8_t regAddress, uint8_t bit_position);

  bool crc_enable = false;

  // Global variables declarations for Burst read function
  uint16_t Burst_reg1; // ADC1 data in case of Burst read(ADC1), FADC1 data when
                       // Burst read(FADC1)
  uint16_t Burst_reg2; // ADC2 data in case of Burst read(ADC1), FADC2 data when
                       // Burst read(FADC1)
  uint16_t Burst_reg3; // ADC3 data in case of Burst read(ADC1), FADC3 data when
                       // Burst read(FADC1)
  uint16_t Burst_reg4; // ADC4 data in case of Burst read(ADC1), FADC4 data when
                       // Burst read(FADC1)
  uint16_t Burst_INT_status;
};

// CRC Table for CRC8 polynomial c(x) = (x8 + x2 + x1 + 1)
unsigned char CRCTable[256] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
    0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
    0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
    0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
    0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
    0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
    0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
    0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
    0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
    0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
    0xfa, 0xfd, 0xf4, 0xf3};

// returns voltage value of content in any of the ADCX, FADCX, COUTX registers
// regAddress - The address of the register to read.
// return - voltage from int value of reg data 100 if not updated from last read
float Convert_to_Voltage(uint8_t regAddress) {
  uint16_t result = 0;
  float voltage_result = 0.0;
  result = MAX2253X::read_register(regAddress);
  if (regAddress == ADC1 || regAddress == ADC2 || regAddress == ADC3 ||
      regAddress == ADC4) {
    if (result > 4096) // 16th bit of ADCx set to 1 meaning ADC not updated
                       // since last read operation
    {
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
