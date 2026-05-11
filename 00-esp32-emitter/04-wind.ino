#ifndef __WIND_SENSOR__
#define __WIND_SENSOR__

#include "99-utils.ino"

#define WIND_DIR_PIN   35
#define WIND_SPEED_PIN 27

#define WIND_READ_DELAY 0

// Debounce for wind speed pin (in microseconds)
#define WIND_SPEED_DEBOUNCE_DELAY 2500UL

// how big is the average buffer
#define WIND_DIR_AVG_BUF    10
// how long to wait between single reads
#define WIND_DIR_READ_DELAY 5

#define WIND_KMH_PER_HZ 2.4f

#define WIND_ERROR_NONE          ERROR_NONE
#define WIND_ERROR_INVALID_INDEX -1

volatile uint32_t wind_last_pulse = 0;
volatile uint32_t wind_last_period = 0;
volatile bool wind_new_period = false;

void IRAM_ATTR windISR(void) {
  uint32_t now = micros();
  uint32_t period = now - wind_last_pulse;
  if (period >= WIND_SPEED_DEBOUNCE_DELAY) {
    wind_last_period = period;
    wind_last_pulse = now;
    wind_new_period = true;
  }
  // else: ignore due to debounce
}


typedef struct WindData {
  float angle;
  float speed;
} WindData;

typedef struct DirectionEntry {
  float angle; // degree
  int   raw;   // hand measured analog read
} DirectionEntry;

// RAW ANALOG MEASURED DATA (averaged)
// `*` marks midpoints between 90 degree jumps
#define DIR_ANGLE_VALUES 16
static DirectionEntry wind_table[] = {
  //  deg. raw
  {   0.0, 3800 }, // pointing inwards
  {  22.5, 2170 },
  {  45.0, 2300 }, // *
  {  67.5,  770 },
  {  90.0,  950 }, // pointing away from green PCB
  { 112.5,  320 },
  { 135.0,  550 }, // *
  { 157.5,   70 },
  { 180.0,  180 }, // pointing outwards
  { 202.5,  140 },
  { 225.0, 1640 }, // *
  { 247.5, 1420 },
  { 270.0, 2930 }, // pointing towards green PCB
  { 292.5, 2500 },
  { 315.0, 3420 }, // *
  { 337.5, 3125 }
};

SetupResult wind_setup() {
  SetupResult result = {
    .min_delay = WIND_READ_DELAY,
    .error     = ERROR_NONE
  };
  pinMode(WIND_DIR_PIN, INPUT);
  pinMode(WIND_SPEED_PIN, INPUT_PULLUP);

attachInterrupt(
      digitalPinToInterrupt(WIND_SPEED_PIN),
      windISR,
      FALLING
   );

  return result;
}

float wind_speed_kmh(uint32_t period) {
  if (period == 0) return 0.0f;
  float freq_hz = 1e6f / period;
  return freq_hz * WIND_KMH_PER_HZ;
}

int wind_dir_raw() {
  int buf = 0;
  for (int i=0; i<WIND_DIR_AVG_BUF; i++) {
    delay(WIND_DIR_READ_DELAY);
    buf += analogRead(WIND_DIR_PIN);
  }
  return buf/WIND_DIR_AVG_BUF;
}

// index of the nearest angle in wind_table to be clear...
int wind_dir_nearest_angle(int raw) {
  int deltas[DIR_ANGLE_VALUES] = {0};

  for (int i=0; i<DIR_ANGLE_VALUES; i++) {
    deltas[i] = abs(raw - wind_table[i].raw);
  }
  // invalid size returns WIND_ERROR_INVALID_INDEX automatically
  return min_idx(deltas, DIR_ANGLE_VALUES);
}

int wind_read(void *dest) {
  WindData *inner = (WindData *) dest;

  /* --- direction --- */
  int raw_dir = wind_dir_raw();
  int angle_idx = wind_dir_nearest_angle(raw_dir);
  if (angle_idx < 0) {
    return WIND_ERROR_INVALID_INDEX;
  } else {
    inner->angle = wind_table[angle_idx].angle;
  }
  /* --- speed --- */
  uint32_t period;
  bool has_new;

  noInterrupts();
  period = wind_last_period;
  has_new = wind_new_period;
  wind_new_period = false;
  interrupts();

  if (has_new && period > 0) {
    inner->speed = wind_speed_kmh(period);
  } else {
    inner->speed = 0.0f;
  }


  return ERROR_NONE;
}



void wind_err_mgr(int status) {
  switch (status) {
    case WIND_ERROR_INVALID_INDEX:
      Serial.println("WIND: An empty array was passed to min_idx!");
      break;
  }
}

#endif // __WIND_SENSOR__
