#ifndef __FLOW_SENSOR__
#define __FLOW_SENSOR__

#include "99-utils.ino"

#define FLOW_PIN 23

#define FLOW_MIN_DELAY 0

#define FLOW_DEBOUNCE_DELAY 2500UL

// mm of water everytime the bucket tilts
#define FLOW_MM_PER_TICK 0.2794

volatile uint32_t flow_last_pulse  = 0;
volatile uint32_t flow_last_period = 0;
volatile bool     flow_new_period  = false;

void IRAM_ATTR flowISR(void) {
  uint32_t now = micros();
  uint32_t period = now - flow_last_pulse;

  if (period >= FLOW_DEBOUNCE_DELAY) {
    flow_last_period = period;
    flow_last_pulse = now;
    flow_new_period = true;
  }
  // else: ignore due to debounce
}

// mm of water per second feels cursed for some reason...
typedef float FlowData; // mm/s

SetupResult flow_setup() {
  SetupResult result = {
    .min_delay = FLOW_MIN_DELAY,
    .error     = ERROR_NONE,
  };

  pinMode(FLOW_PIN, INPUT_PULLUP);
  attachInterrupt(
      digitalPinToInterrupt(FLOW_PIN),
      flowISR,
      FALLING
   );

  return result;
}

float flow_rate(uint32_t period) {
  if (period == 0) return 0.0f;
  float seconds = period / 1e6f;
  return FLOW_MM_PER_TICK / seconds;
}

int flow_read(void *dest) {
  FlowData *inner = (FlowData *) dest;

  uint32_t period;
  bool has_new;

  noInterrupts();
  period = flow_last_period;
  has_new = flow_new_period;
  wind_new_period = false;
  interrupts();

  if (has_new && period > 0) {
    *inner = flow_rate(period);
  } else {
    *inner = 0.0f;
  }

  return ERROR_NONE;
}

void flow_err_mgr(int status) {
  switch (status) {
  }
}

#endif // __FLOW_SENSOR__
