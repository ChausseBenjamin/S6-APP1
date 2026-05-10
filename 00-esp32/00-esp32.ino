#define BAUD_RATE 9600

unsigned long loopStart;

int min_delay;

void setup() {
  // put your setup code here, to run once :
  Serial.begin(BAUD_RATE);
  loopStart = millis();

  delay(1000);
  Serial.println("Setting up devices...");

  min_delay += light_setup();
  min_delay += humidity_setup();


  Serial.printf("Setup Completed in %dms\n", elapsed() );
}

void loop() {
  loopStart = millis();

  HumidityData humidity_data;

  int light_data = light_read();
  humidity_read(&humidity_data);
  log_humidity_errors(humidity_data->error);



}
