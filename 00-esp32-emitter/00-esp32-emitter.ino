#include "01-light.ino"
#include "02-humidity.ino"
#include "03-pressure.ino"
#include "04-wind.ino"

#include "99-utils.ino"
#include "98-UART-slave.ino"
#include "97-BLE-station.ino"

int global_min_delay = 0;
UARTData weather;

SensorModule sensors[] = {
  { light_setup,    light_read,    light_err_mgr    },
  { wind_setup,     wind_read,     wind_err_mgr     },
  { humidity_setup, humidity_read, humidity_err_mgr },
  { pressure_setup, pressure_read, pressure_err_mgr },
};
int module_count = LENGTH(sensors);

// Avoid constantly allocating+freeing:
// Always rewrite to the same memory location on every read
int          light_data;
WindData     wind_data;
HumidityData humidity_data;
PressureData pressure_data;
// XXX: ensure commented sensors are also commented here to ensure alignment
// XXX: ensure destinations have the same order as their parent sensor
//      (otherwise: Memory-Leak go brrrrr!)
void *destinations[] = {
  &light_data,
  &wind_data,
  &humidity_data,
  &pressure_data,
};

void setup() {
  Serial.begin(BAUD_RATE);
  delay(SERIAL_CONN_AWAIT);
  Serial.println("Setting up devices...");

  for (int i=0; i<module_count; i++) {
    // run the setup func
    SetupResult r = sensors[i].init();
    global_min_delay = max(global_min_delay, r.min_delay);
    sensors[i].manage_errors(r.error);
  }

  uart_slave_setup();
  ble_station_setup();

  Serial.printf("Setup Completed in %dms\n", elapsed());
}

void loop() {
  loopStart = millis();

  for (int i=0; i<module_count; i++) {
    int err = sensors[i].update(destinations[i]);
    if (err != NIL) {
      sensors[i].manage_errors(err);
    }
  }

  Serial.printf("light:%d,humidity:{H:%f,T:%f},pressure:{P:%f,T:%f},wind:{dir:%f,speed:%f}\n",
    light_data,
    humidity_data.humidity,
    humidity_data.temp,
    pressure_data.pressure,
    pressure_data.temp,
    wind_data.angle,
    wind_data.speed
  );

  // BLE Notifications
  weather.temperature = humidity_data.temp;
  weather.humidity = humidity_data.temp;
  weather.wind_speed = wind_data.speed;
  weather.wind_direction = wind_data.angle;
  weather.pressure = pressure_data.pressure;
  weather.light = light_data;
  weather.precipitation = 0;


  weather.error = UART_ERROR_NONE;
  ble_notify(&weather);
  uart_slave_answer(&weather);
  uart_err_mgr(weather.error);

  while (elapsed() < global_min_delay) {
    delay(1); // Service the watchdog
  }
}
