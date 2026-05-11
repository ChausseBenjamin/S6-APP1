#ifndef __UTILS__
#define __UTILS__

// ---- MISC

#define BAUD_RATE         115200
#define SERIAL_CONN_AWAIT 1000
#define ERROR_NONE        0
// I like golang okay...
#define NIL               0

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

int min_idx(const int *arr, size_t len) {
  if (len == 0) return -1;

  int min_val = INT_MAX;
  int min_idx = 0;

  for (size_t i = 0; i < len; i++) {
    if (arr[i] < min_val) {
      min_val = arr[i];
      min_idx = (int)i;
    }
  }

  return min_idx;
}

// ---- SETUP API

typedef struct SetupResult {
  int min_delay;
  int error;
} SetupResult;

// any function a sensor uses for its setup
typedef SetupResult (*setup_fn_t)(void);
// any function a sensor uses to read/update sensor data
// NOTE: it MUST cast *dest back into the correct struct
//       before writing...
// returns error code (or ERROR_NONE)
typedef int (*reader_fn_t)(void *);
// any function a sensor uses to log shit
typedef void (*err_mgr_fn_t)(int);

typedef struct SensorModule {
  setup_fn_t   init;
  reader_fn_t  update;
  err_mgr_fn_t manage_errors;
} SensorModule;

// ---- TIMING STUFF

unsigned long loopStart;

unsigned long elapsed() {
  return millis() - loopStart;
}

unsigned long elapsed_since(unsigned long since) {
  return millis() - since;
}

#endif // __UTILS__
