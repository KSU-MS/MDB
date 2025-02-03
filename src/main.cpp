#include "main.hpp"

// Define gizmos
Chrono can_5hz;
Chrono can_10s;

MAX2253X a_temp;
MAX2253X b_temp;

// These hold the data for the CAN messages
uint8_t buf_voltage[8];

uint16_t time_last;

// Init
void setup() {
  Serial.begin(9600);

  Serial.print("Temp A Init");
  a_temp.init(A_CS);

  Serial.print("\tTemp B Init\n");
  b_temp.init(B_CS);

  Serial.println("CAN init");
  CAN.begin(CAN_BPS_500K);

  Serial.println("Starting...");

#ifdef DEBUG
  Serial.println("DEBUG tools enabled");
#endif
}

// Evil main loop
// TODO: Make this not evil (move guys to other funcs to make it readable)
void loop() {
  // We don't want to nuke the bus, thus we limit how often the temps are sent
  if (can_5hz.hasPassed(200)) {
    can_5hz.restart();

#ifdef FORWARD
    buf_voltage[0] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC1) * 100);
    buf_voltage[1] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC2) * 100);
    buf_voltage[2] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC3) * 100);
    buf_voltage[3] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC4) * 100);

    buf_voltage[4] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC4) * 100);
    buf_voltage[5] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC3) * 100);
    buf_voltage[6] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC2) * 100);
    buf_voltage[7] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC1) * 100);
#endif

#ifndef FORWARD
    buf_voltage[0] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC1) * 100);
    buf_voltage[1] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC2) * 100);
    buf_voltage[2] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC3) * 100);
    buf_voltage[3] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC4) * 100);

    buf_voltage[4] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC4) * 100);
    buf_voltage[5] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC3) * 100);
    buf_voltage[6] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC2) * 100);
    buf_voltage[7] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC1) * 100);
#endif

#ifdef DEBUG
    Serial.print("ADC 1: ");
    Serial.print(buf_voltage[0] * .01);
    Serial.print("\tADC 2: ");
    Serial.print(buf_voltage[1] * .01);
    Serial.print("\tADC 3: ");
    Serial.print(buf_voltage[2] * .01);
    Serial.print("\tADC 4: ");
    Serial.print(buf_voltage[3] * .01);
    Serial.print("\tADC 5: ");
    Serial.print(buf_voltage[4] * .01);
    Serial.print("\tADC 6: ");
    Serial.print(buf_voltage[5] * .01);
    Serial.print("\tADC 7: ");
    Serial.print(buf_voltage[6] * .01);
    Serial.print("\tADC 8: ");
    Serial.println(buf_voltage[7] * .01);
#endif

    CAN.write(id_a, CAN_STANDARD_FRAME, 8, buf_voltage);
  }

  // This guy aint that important, send him every 10 seconds
  if (can_10s.hasPassed(10000)) {
    can_10s.restart();

    // Update our time part of the message
    set_time();

    // Temp and humidity datasheet, page 2 has the voltage to unit conversions
    // https://use.365.altium.com/librarycomponentsapi/api/v1/References/10EA8973-BADC-4062-8AF9-890FE0995B5D
    // VDD=5.0V 22.9 mV/°C, The pro micro does 10bit reads, but value shouldn't
    // read above 100 so cast to uint8_t and ball. So get our shit into volts
    // 2^10 - 1 = 1023 -> 5.0/1023 = 0.00488758553275
    // Get our volts into degrees
    // 1/(22.9 * 10^-3) = 43.6681222707
    // Multiply the two and now we have a factor for uint16 -> degrees

    // VDD=5.0V 22.9 mV/°C
    uint8_t temp = uint8_t(analogRead(Ambient_Temp) * 0.213431682653);
    set_temp(temp);

    // VDD=5.0V 40.0 mV/%RH
    uint8_t humidity = uint8_t(analogRead(Ambient_Humid) * 0.122189638319);
    set_humidity(humidity);

    CAN.write(id_b, CAN_STANDARD_FRAME, 8, board_status);
  }
}
