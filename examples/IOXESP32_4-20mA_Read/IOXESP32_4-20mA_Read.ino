#include <Arduino.h>
#include <Wire.h>
#include <IOXESP32_4-20mA_Receiver.h>

Receiver4_20 sensor(&Wire, 0x45); // Default 0x45

void setup() {
  Serial.begin(115200);

  Wire.begin(); // Start I2C
  while (!sensor.begin()) { // Start ADC
    Serial.println("Not found hardware :(");
    delay(2000);
  }
}

void loop() {
  delay(100);
  if (sensor.measure()) { // Update analog value from ADC
    int16_t raw = sensor.raw(); // Read raw value (-32768 to 32767)
    float current = sensor.current(); // Read current in mA unit
    Serial.printf("Raw=%5d   Current=%.02f mA\n", raw, current);
  } else {
    Serial.println("Hardware error !");
  }
}
