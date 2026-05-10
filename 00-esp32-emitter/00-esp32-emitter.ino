#include "01-light.ino"
#include "02-humidity.ino"
#include "03-pressure.ino"
#include "99-utils.ino"
#include "98-UART-slave.ino"

int global_min_delay = 0;

// Avoid constantly allocating+freeing:
// Always rewrite to the same memory location on every read
int light_data;
HumidityData humidity_data;
PressureData pressure_data;

void setup() {
  Serial.begin(BAUD_RATE);
  delay(SERIAL_CONN_AWAIT);
  Serial.println("Setting up devices...");

  Module modules[] = {
    { light_setup,    light_error_manager    },
    { humidity_setup, humidity_error_manager },
    { pressure_setup, pressure_error_manager },
  };

  // Automated setup for every module
  int module_count = LENGTH(modules);
  for (int i=0; i<module_count; i++) {
    // run the setup func
    SetupResult r = modules[i].init();
    global_min_delay = max(global_min_delay, r.min_delay);
    modules[i].log_errors(r.error);
  }

  Serial.printf("Setup Completed in %dms\n", elapsed());
}

void loop() {
  loopStart = millis();

  light_read(&light_data);
  humidity_read(&humidity_data);
  pressure_read(&pressure_data);

  humidity_error_manager(humidity_data.error);
  pressure_error_manager(pressure_data.error);

  Serial.printf("light:%d,humidity:{H:%f,T:%f},pressure:{P:%f,T:%f}\n",
    light_data,
    humidity_data.humidity,
    humidity_data.temp,
    pressure_data.pressure,
    pressure_data.temp);

  while (elapsed() < global_min_delay);
}
