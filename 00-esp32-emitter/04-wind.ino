#ifndef __WIND_SENSOR__
#define __WIND_SENSOR__

#include "99-utils.ino"

#define WIND_DIR_PIN   35
#define WIND_SPEED_PIN 27

#define WIND_READ_DELAY 0

// how big is the average buffer
#define WIND_DIR_AVG_BUF    10
// how long to wait between single reads
#define WIND_DIR_READ_DELAY 5

#define WIND_ERROR_NONE          ERROR_NONE
#define WIND_ERROR_INVALID_INDEX -1

typedef struct WindData {
  float angle;
  float speed;
  int   error;
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
  // TODO: setup speed at some point

  pinMode(WIND_DIR_PIN, INPUT);
  return result;
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

void wind_read(WindData *dest) {
  dest->error = ERROR_NONE;

  int raw_dir = wind_dir_raw();
  int angle_idx = wind_dir_nearest_angle(raw_dir);
  if (angle_idx < 0) {
    dest->error = WIND_ERROR_INVALID_INDEX;
  } else {
    dest->angle = wind_table[angle_idx].angle;
  }
  // TODO: setup speed at some point
}



void wind_error_manager(int status) {
  switch (status) {
    case WIND_ERROR_INVALID_INDEX:
      Serial.println("WIND: An empty array was passed to min_idx!");
      break;
  }
}

#endif // __WIND_SENSOR__
