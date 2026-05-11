#ifndef __PRESSURE_SENSOR__
#define __PRESSURE_SENSOR__

#include "99-utils.ino"
#include <Wire.h>
#include <Adafruit_DPS310.h>

// We measure pressure in PSIs... Idk for real...
#define PSI_SDA_PIN 21
#define PSI_SCL_PIN 22

#define PSI_READ_DELAY 1000

#define PSI_ERROR_NONE      ERROR_NONE
#define PSI_ERROR_NO_DEVICE 1

Adafruit_DPS310 dps;

typedef struct PressureData {
  float temp;
  float pressure;
  int   error;
} PressureData;

SetupResult pressure_setup() {
  SetupResult result = {
    .min_delay = PSI_READ_DELAY,
    .error     = ERROR_NONE
  };
  Wire.begin(PSI_SDA_PIN,PSI_SCL_PIN);

  if (!dps.begin_I2C()) {
    result.error = PSI_ERROR_NO_DEVICE;
    return result;
  }

  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  return result;
}

int pressure_read(void *dest) {
  PressureData *inner = (PressureData *) dest;

  sensors_event_t temp_event, psi_event;
  dps.getEvents(&temp_event, &psi_event);

  inner->temp     = temp_event.temperature;
  inner->pressure = psi_event.pressure *100; // hPa->Pa
  return ERROR_NONE;
}

void pressure_err_mgr(int status) {
  switch (status) {
    case PSI_ERROR_NO_DEVICE:
      Serial.println("PRESSURE: Missing DPS310 Sensor!");
      break;
  }
}

#endif // __PRESSURE_SENSOR__
