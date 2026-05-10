#define LIGHT_PIN 34

// 2 in 1: also returns the minimum delay needed
// between reads.
int light_setup() {
  pinMode(LIGHT_PIN, INPUT);
  return 0;
}

int light_read() {
  int val = analogRead(LIGHT_PIN);
  return val;
}
