#ifndef __UTILS__
#define __UTILS__

// ---- MISC
#define BAUD_RATE         115200
#define SERIAL_CONN_AWAIT 1000
#define ERROR_NONE        0

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

// ---- SETUP API

typedef struct SetupResult {
  int min_delay;
  int error;
} SetupResult;

// any function a sensor uses to log shit
typedef void (*log_fn_t)(int);
// any function a sensor uses for its setup
typedef SetupResult (*setup_fn_t)(void);

typedef struct Module {
  setup_fn_t init;
  log_fn_t   log_errors;
  // I don't put a generic *reader* struct because every sensor
  // has it's own return structures so I'd need
  // to deal with pointer dereferencing bullshit to make
  // a *clean* generalized solution work...
  // Not worth it. Cas-par-cas it is for reading data.
} Module;

// ---- TIMING STUFF
unsigned long loopStart;

unsigned long elapsed() {
  unsigned long val = millis() - loopStart;
  return val;
}

// ---- ERRROR MANAGEMENT

typedef void (*error_logger_fn)(int);

void manage_errors(error_logger_fn logger, int status) {
  if (logger != NULL) {
    logger(status);
  }
}

#endif // __UTILS__
