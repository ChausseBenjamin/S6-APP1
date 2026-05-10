#include <Wire.h>
#include <Adafruit_DPS310.h>

Adafruit_DPS310 dps;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL

  if (!dps.begin_I2C()) {
    Serial.println("Erreur: capteur DPS310 non détecté !");
    while (1);
  }

  Serial.println("Capteur DPS310 initialisé");

  // Configuration
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
}

void loop() {
  sensors_event_t temp_event, pressure_event;

  dps.getEvents(&temp_event, &pressure_event);

  Serial.print("Temperature: ");
  Serial.print(temp_event.temperature);
  Serial.println(" °C");

  Serial.print("Pression: ");
  Serial.print(pressure_event.pressure * 100); // hPa → Pa
  Serial.println(" Pa");

  Serial.println("-----------------------");

  delay(1000);
}
