#include "main.hpp"
#include "Arduino.h"
#include "git.hpp"

// Set target module
uint16_t id_a = MODULE_1_A;
uint16_t id_b = MODULE_1_B;

// This just sets whether or not the positive end of the board is facing the
// front of the car or the rear so that the temp data is relative to the car
#define FORWARD

// Define gizmos
Chrono can_20hz;
Chrono can_10s;

MAX2253X a_temp;
MAX2253X b_temp;

// These hold the data for the CAN messages
uint8_t buf_a[8];
uint8_t buf_b[8];

uint16_t time_last;

// Init
void setup() {
  Serial.begin(9600);

  Serial.print("Temp A Init");
  a_temp.init(A_CS);

  Serial.print("\tTemp B Init\n");
  b_temp.init(B_CS);

  Serial.println("CAN init");
  init_CAN();

  Serial.print("Git hash: ");
  Serial.println(status_message.firmware_version);
  Serial.print("Is main: ");
  Serial.print(status_message.project_on_main_or_master);
  Serial.print("\tIs dirty: ");
  Serial.println(status_message.project_is_dirty);

  Serial.println("Starting...");
  Serial.println(sizeof(status_message));
}

// Evil main loop
// TODO: Make this not evil (move guys to other funcs to make it readable)
void loop() {
  // We don't want to nuke the bus, thus we limit how often the temps are sent
  if (can_20hz.hasPassed(50)) {
    can_20hz.restart();

#ifdef FORWARD
    buf_a[0] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC1) * 100);
    buf_a[1] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC2) * 100);
    buf_a[2] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC3) * 100);
    buf_a[3] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC4) * 100);

    buf_a[4] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC4) * 100);
    buf_a[5] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC3) * 100);
    buf_a[6] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC2) * 100);
    buf_a[7] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC1) * 100);
#endif

#ifndef FORWARD
    buf_a[0] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC1) * 100);
    buf_a[1] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC2) * 100);
    buf_a[2] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC3) * 100);
    buf_a[3] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC4) * 100);

    buf_a[4] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC4) * 100);
    buf_a[5] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC3) * 100);
    buf_a[6] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC2) * 100);
    buf_a[7] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC1) * 100);
#endif

#ifdef DEBUG
    Serial.print("A_ADC 1: ");
    Serial.print(buf_a[0] * .01);
    Serial.print("\tA_ADC 2: ");
    Serial.print(buf_a[1] * .01);
    Serial.print("\tA_ADC 3: ");
    Serial.print(buf_a[2] * .01);
    Serial.print("\tA_ADC 4: ");
    Serial.println(buf_a[3] * .01);

    Serial.print("B_ADC 1: ");
    Serial.print(buf_a[4] * .01);
    Serial.print("\tB_ADC 2: ");
    Serial.print(buf_a[5] * .01);
    Serial.print("\tB_ADC 3: ");
    Serial.print(buf_a[6] * .01);
    Serial.print("\tB_ADC 4: ");
    Serial.println(buf_a[7] * .01);
#endif

    send_CAN(id_a, 8, buf_a);
  }

  // This guy aint that important, send him every 10 seconds
  if (can_10s.hasPassed(10000)) {
    can_10s.restart();

    status_message.on_time_seconds = uint16_t((millis() / 1000));

    // Temp and humidity datasheet, page 2 has the voltage to unit conversions
    // https://use.365.altium.com/librarycomponentsapi/api/v1/References/10EA8973-BADC-4062-8AF9-890FE0995B5D
    // The pro micro has a 10bit ADC, and a voltage range of 0-5v, so 5/2^10
    // gives us the v/int, so all we have to do is multiply by 0.0048828125 to
    // convert the analogRead function back to a voltage, then divide that by
    // the guys unit conversion provided by the datasheet and boom, arbitrary
    // voltage converted to arbitrary number converted back to an arbitrary
    // voltage, and then converted to a usable unit. Simple, right?
    // I am then going to combine the v/int with the mV/%RH just to make the
    // code cleaner, but that is where these factors come from.

    // VDD=5.0V 40.0 mV/%RH
    set_humidity(uint8_t(analogRead(Ambient_Humid) * 0.1220703125));

    // VDD=5.0V 22.9 mV/°C
    set_temp(uint8_t(analogRead(Ambient_Temp) * 0.213223253275));

    memcpy(buf_b, status_message.b, 8);

    send_CAN(id_b, 8, buf_b);
  }
}
