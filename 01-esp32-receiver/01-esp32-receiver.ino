#include "00-UART-master.ino"
#include "02-BLE-server.ino"
#include "99-utils.ino"

int min_delay = 0;
UARTData uart_data;

void setup() {
  Serial.begin(BAUD_RATE);
  delay(AWAIT_SERIAL_SETUP_MS);
  Serial.println("Setting up devices...");

  min_delay = max(uart_master_setup(), min_delay);
  min_delay = max(ble_server_setup(), min_delay);

  Serial.printf("Setup Completed in %dms\n", elapsed());
}

void loop() {
  loopStart = millis();

  uart_master_read(&uart_data);

  log_UART_errors(uart_data.error);
  Serial.println("Received data:");
  Serial.println(uart_data.raw);

  while (elapsed() < min_delay);
}
