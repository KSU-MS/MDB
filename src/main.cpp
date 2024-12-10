#include "main.hpp"
#include "CAN_Handle.hpp"

// Define
Chrono can_20hz;
MAX2253X a_temp;
MAX2253X b_temp;
uint16_t id_a = MODULE_1_A;
// uint16_t id_b = MODULE_1_B;
uint8_t buf_a[8];

// Init
void setup() {
  Serial.begin(9600);

  delay(3000);

  Serial.print("Temp A Init");
  a_temp.init(A_CS);

  Serial.print("\tTemp B Init\n");
  b_temp.init(B_CS);

  delay(3000);

  Serial.println("CAN init");
  init_CAN();

  delay(1500);
  Serial.println("Starting...");
}

// Execute
void loop() {
  if (can_20hz.hasPassed(50)) {
    can_20hz.restart();

    Serial.print("A_ADC 1: ");
    buf_a[0] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC1) * 100);
    Serial.print(buf_a[0] * .01);
    Serial.print("\tA_ADC 2: ");
    buf_a[1] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC2) * 100);
    Serial.print(buf_a[1] * .01);
    Serial.print("\tA_ADC 3: ");
    buf_a[2] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC3) * 100);
    Serial.print(buf_a[2] * .01);
    Serial.print("\tA_ADC 4: ");
    buf_a[3] = uint8_t(a_temp.Convert_to_Voltage(a_temp.ADC4) * 100);
    Serial.println(buf_a[3] * .01);

    Serial.print("B_ADC 1: ");
    buf_a[4] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC1) * 100);
    Serial.print(buf_a[4] * .01);
    Serial.print("\tB_ADC 2: ");
    buf_a[5] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC2) * 100);
    Serial.print(buf_a[5] * .01);
    Serial.print("\tB_ADC 3: ");
    buf_a[6] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC3) * 100);
    Serial.print(buf_a[6] * .01);
    Serial.print("\tB_ADC 4: ");
    buf_a[7] = uint8_t(b_temp.Convert_to_Voltage(b_temp.ADC4) * 100);
    Serial.println(buf_a[7] * .01);

    send_CAN(id_a, 8, buf_a);
  }
}
