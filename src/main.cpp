#include "main.hpp"

// Define
Chrono can_20hz;
MAX2253X a_temp;
MAX2253X b_temp;
uint16_t id_a = MODULE_1_A;
uint16_t id_b = MODULE_1_B;
uint16_t guy;

// Init
void setup() {
  Serial.begin(9600);

  delay(3000);

  Serial.print("Temp A Init");
  a_temp.init(B_CS);

  // Serial.print("\tTemp B Init\n");
  // b_temp.init(B_CS);

  Serial.println("Begining print test");

  delay(3000);
}

// Execute
void loop() {
  if (can_20hz.hasPassed(1500)) {
    can_20hz.restart();

    Serial.print("A_ADC 1: ");
    Serial.print(a_temp.read_register(MAX2253X::ADC1));
    Serial.print("\tA_ADC 2: ");
    Serial.print(a_temp.read_register(MAX2253X::ADC2));
    Serial.print("\tA_ADC 3: ");
    Serial.print(a_temp.read_register(MAX2253X::ADC3));
    Serial.print("\tA_ADC 4: ");
    Serial.println(a_temp.read_register(MAX2253X::ADC4));
    // Serial.print("B_ADC 1: ");
    // Serial.print(b_temp.read_register(MAX2253X::ADC1));
    // Serial.print("\tB_ADC 2: ");
    // Serial.print(b_temp.read_register(MAX2253X::ADC2));
    // Serial.print("\tB_ADC 3: ");
    // Serial.print(b_temp.read_register(MAX2253X::ADC3));
    // Serial.print("\tB_ADC 4: ");
    // Serial.println(b_temp.read_register(MAX2253X::ADC4));
  }
}
