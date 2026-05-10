#ifndef __UTILS__
#define __UTILS__

#define BAUD_RATE 115200            // Comms with debug consol
#define AWAIT_SERIAL_SETUP_MS 2000  // Time to wait, so debug consol don't miss first exchanges

#define ERROR_NONE 0;

unsigned long loopStart;

unsigned long elapsed() {
  unsigned long val = millis() - loopStart;
  return val;
}

#endif // __UTILS__
