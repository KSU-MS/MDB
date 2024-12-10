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
  a_temp.init(A_CS);

  Serial.print("\tTemp B Init\n");
  b_temp.init(B_CS);

  Serial.println("Begining print test");

  delay(3000);
}

// Execute
void loop() {
  if (can_20hz.hasPassed(1500)) {
    can_20hz.restart();

    Serial.print("ADC1: ");
    Serial.print(a_temp.read_register(MAX2253X::ADC1));
    Serial.print("\tADC2: ");
    Serial.print(a_temp.read_register(MAX2253X::ADC2));
    Serial.print("\tADC3: ");
    Serial.print(a_temp.read_register(MAX2253X::ADC3));
    Serial.print("\tADC4: ");
    Serial.println(a_temp.read_register(MAX2253X::ADC4));
  }
}
