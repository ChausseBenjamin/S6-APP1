#define HUMID_PIN 16

#define HUMID_ERROR_NONE     0
#define HUMID_ERROR_TIMING   1
#define HUMID_ERROR_CHECKSUM 2

typedef struct HumidityData {
  float temp;
  float humidity;
  int error;
} HumidityData;

// 2 in 1: also returns the minimum delay needed
// between reads.
int humidity_setup() {
  pinMode(HUMID_PIN, INPUT);
  return 2000;
}

void humidity_read(HumidityData *dest) {
  int i, j;
  int duration[42];
  unsigned long pulse;
  byte data[5];
  float humidite;
  float temperature;
  int broche = 16;

  // clean struct pointer:
  dest->temp = 0;
  dest->humidity = 0;
  dest->error = HUMID_ERROR_NONE;

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
        duration[i] = pulse;
        i++;
  } while (pulse != 0);

  if (i != 42) {
    dest->error = HUMID_ERROR_TIMING;
    return ;
  };

  for (i=0; i<5; i++) {
    data[i] = 0;
    for (j = ((8*i)+1); j < ((8*i)+9); j++) {
      data[i] = data[i] * 2;
      if (duration[j] > 50) {
        data[i] = data[i] + 1;
      }
    }
  }

  if ( (data[0] + data[1] + data[2] + data[3]) != data[4] ) {
    dest->error = HUMID_ERROR_CHECKSUM;
    return ;
  }

  dest->humidity = data[0] + (data[1] / 256.0);
  dest->temp = data [2] + (data[3] / 256.0);

}

void log_humidity_errors(int status) {
  switch (status) {
    case HUMID_ERROR_TIMING:
      Serial.println("HUMIDITY: Timing error!");
    case HUMID_ERROR_CHECKSUM:
      Serial.println("HUMIDITY: Checksum error!");
  }
}
