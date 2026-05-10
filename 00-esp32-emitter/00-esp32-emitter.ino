#include "01-light.ino"
#include "02-humidity.ino"
#include "99-utils.ino"

int min_delay = 0;

int light_data;
HumidityData humidity_data;

void setup() {
  Serial.begin(BAUD_RATE);
  delay(1000);
  Serial.println("Setting up devices...");
  min_delay = 0;

  min_delay = max(light_setup(),    min_delay);
  min_delay = max(humidity_setup(), min_delay);

  Serial.printf("Setup Completed in %dms\n", elapsed());
}

void loop() {
  loopStart = millis();

  light_data = light_read();

  humidity_read(&humidity_data);
  log_humidity_errors(humidity_data.error);

  Serial.printf("Light: %d, Humidity{H: %f, Temp: %f}\n",
    light_data,
    humidity_data.humidity,
    humidity_data.temp);

  while (elapsed() < min_delay);
}
