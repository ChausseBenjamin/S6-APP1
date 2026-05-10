#ifndef __HUMIDITY_SENSOR__
#define __HUMIDITY_SENSOR__

#include "99-utils.ino"

#define HUMID_PIN 16

#define HUMID_READ_DELAY 2000

#define HUMID_ERROR_NONE     ERROR_NONE
#define HUMID_ERROR_TIMING   1
#define HUMID_ERROR_CHECKSUM 2

typedef struct HumidityData {
  float temp;
  float humidity;
  int error;
} HumidityData;

// ---- humidity-sensor.ino ----
SetupResult humidity_setup() {
  SetupResult result = {
    .min_delay = HUMID_READ_DELAY,
    .error     = ERROR_NONE
  };

  // Do NOT set pinMode here; DHT protocol does it per-read

  return result;
}

void humidity_read(HumidityData *dest) {
  int i, j;
  int duree[42];
  unsigned long pulse;
  byte data[5];
  dest->temp = 0;
  dest->humidity = 0;
  dest->error = HUMID_ERROR_NONE;

  // REFERENCE: Working labo sketch logic
  delay(2); // tiny pause before init (was delay(2000) in loop in sample)

  pinMode(HUMID_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(HUMID_PIN, HIGH);
  delay(250);
  digitalWrite(HUMID_PIN, LOW);
  delay(20);
  digitalWrite(HUMID_PIN, HIGH);
  delayMicroseconds(40);
  pinMode(HUMID_PIN, INPUT_PULLUP);

  while (digitalRead(HUMID_PIN) == HIGH);
  i = 0;

  do {
    pulse = pulseIn(HUMID_PIN, HIGH);
    duree[i] = pulse;
    i++;
  } while (pulse != 0 && i < 42);

  if (i != 42) {
    dest->error = HUMID_ERROR_TIMING;
    return;
  }

  for (i = 0; i < 5; i++) {
    data[i] = 0;
    for (j = ((8 * i) + 1); j < ((8 * i) + 9); j++) {
      data[i] = data[i] * 2;
      if (duree[j] > 50) {
        data[i] = data[i] + 1;
      }
    }
  }

  if ((data[0] + data[1] + data[2] + data[3]) != data[4]) {
    dest->error = HUMID_ERROR_CHECKSUM;
    return;
  }

  dest->humidity = data[0] + (data[1] / 256.0);
  dest->temp = data[2] + (data[3] / 256.0);
}

void humidity_error_manager(int status) {
  switch (status) {
    case HUMID_ERROR_TIMING:
      Serial.println("HUMIDITY: Timing error!");
      break;
    case HUMID_ERROR_CHECKSUM:
      Serial.println("HUMIDITY: Checksum error!");
      break;
  }
}

#endif // __HUMIDITY_SENSOR__
