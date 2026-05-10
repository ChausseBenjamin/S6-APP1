#include "01-light.ino"
#include "02-humidity.ino"

#include "99-utils.ino"

int min_delay = 0;

void setup() {
  Serial.begin(BAUD_RATE);
  loopStart = millis();

  delay(1000);
  Serial.println("Setting up devices...");

  // If we don't want to deal with modulo shit and a scheduler,
  // just rate limit the main loop to the slowest sensor.
  //
  // Yeah, we don't get up-to-date info on quick sensors...
  // for an APP -> Minimum Viable Product
  min_delay = max(min_delay, light_setup());
  min_delay = max(min_delay, humidity_setup());

  Serial.printf("Setup Completed in %dms\n", elapsed() );
}

void loop() {
  loopStart = millis();

  HumidityData humidity_data;

  int light_data = light_read();
  humidity_read(&humidity_data);
  log_humidity_errors(humidity_data.error);

  Serial.printf("Light: %d, Humidity{H: %f, Temp: %f}\n",
      light_data,
      humidity_data.humidity,
      humidity_data.temp);

  while (elapsed() < min_delay) ;

}
