#include "main.hpp"
#include "CAN_Handle.hpp"
#include "ksu_device_status.h"

// Set target module
uint16_t id_a = MODULE_1_A;
uint16_t id_b = MODULE_1_B;

// Define gizmos
Chrono can_20hz;
Chrono can_10s;

device_status_t board_status_t;

MAX2253X a_temp;
MAX2253X b_temp;

// These hold the data for the CAN messages
uint8_t buf_a[8];
uint8_t buf_b[8];

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
  Serial.println(board_status_t.firmware_version);
  Serial.print("Is main: ");
  Serial.print(board_status_t.project_on_main_or_master);
  Serial.print("\tIs dirty: ");
  Serial.println(board_status_t.project_is_dirty);

  Serial.println("Starting...");
  Serial.println(sizeof(device_status_t));
}

// Execute
void loop() {
  // We don't want to nuke the bus, thus we limit how often the temps are sent
  if (can_20hz.hasPassed(50)) {
    can_20hz.restart();

    buf_a[0] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC1) * 100);
    buf_a[1] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC2) * 100);
    buf_a[2] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC3) * 100);
    buf_a[3] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC4) * 100);

    buf_a[4] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC1) * 100);
    buf_a[5] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC2) * 100);
    buf_a[6] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC3) * 100);
    buf_a[7] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC4) * 100);

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

  if (can_10s.hasPassed(10000)) {
    can_10s.restart();

    // send_CAN(id_b, 8, buf_b);
  }
}
