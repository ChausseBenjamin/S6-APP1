#ifndef __LIGHT_SENSOR__
#define __LIGHT_SENSOR__

#include "99-utils.ino"

#define LIGHT_PIN 34

#define LIGHT_READ_DELAY 0

SetupResult light_setup() {
  SetupResult result = {
    .min_delay = LIGHT_READ_DELAY,
    .error     = ERROR_NONE
  };
  pinMode(LIGHT_PIN, INPUT);
  return result;
}

int light_read() {
  int val = analogRead(LIGHT_PIN);
  return val;
}

void light_error_manager(int status) {
  // doesn't have errors...
  // just to stay consistent with other devices
  return ;
}

#endif // __LIGHT_SENSOR__
