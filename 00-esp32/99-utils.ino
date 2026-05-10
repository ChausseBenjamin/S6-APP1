#ifndef __UTILS__
#define __UTILS__

#define BAUD_RATE 115200

#define ERROR_NONE 0;

unsigned long loopStart;

unsigned long elapsed() {
  unsigned long val = millis() - loopStart;
  return val;
}

#endif // __UTILS__
