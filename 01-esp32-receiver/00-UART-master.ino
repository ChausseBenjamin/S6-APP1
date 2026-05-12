#ifndef __00_UART_MASTER__
#define __00_UART_MASTER__

#define PIN_RX 16
#define PIN_TX 17

#define UART_REQUEST            "Get"
#define UART_TIMEOUT_MS         2000
#define UART_BAUD_RATE          9600
#define UART_END_CHAR           '\n'
#define UART_START_CHAR         '\1'
#define UART_REQUEST_GAP_MS     20 // How long you wait before you can request again.
#define UART_MESSAGE_MAX_LENGTH 100

#define UART_ERROR_NONE             0
#define UART_ERROR_TIMEOUT          1
#define UART_ERROR_MESSAGE_TOO_LONG 2
#define UART_ERROR_NO_START         3
#define UART_ERROR_NO_ANSWER        4

HardwareSerial UART(2);

typedef struct UARTData {
  String raw;
  int error;
} UARTData;

int uart_master_setup() {
  UART.begin(UART_BAUD_RATE, SERIAL_8N1, PIN_RX, PIN_TX);
  return UART_REQUEST_GAP_MS;
}

void uart_master_read(UARTData *dest) {
  Serial.println("UART: Sending request");

  UART.println(UART_REQUEST);

  unsigned long start_time = millis();
  bool received_things = false;
  bool seen_start_char = false;
  String message = "";

  while (millis() - start_time < UART_TIMEOUT_MS) {
    while (UART.available()) {
      received_things = true;
      char received = UART.read();

      if (!seen_start_char) {
        if (received == UART_START_CHAR) {
          seen_start_char = true;
        }
      } else {
        if (received == UART_END_CHAR) {
          dest->raw = message;
          dest->error = UART_ERROR_NONE;
          return;
        }

        message += received;

        // Prevents memory busting in case of error
        if (message.length() > UART_MESSAGE_MAX_LENGTH) {
          dest->raw = message;
          dest->error = UART_ERROR_MESSAGE_TOO_LONG;
          return;
        }
      }
    }
  }

  if (!received_things) {
    dest->error = UART_ERROR_NO_ANSWER;
    dest->raw = "";
    return;
  }

  if (!seen_start_char) {
    dest->error = UART_ERROR_NO_START;
    dest->raw = "";
  }

  dest->error = UART_TIMEOUT_MS;
  dest->raw = message;
}

void log_UART_errors(int status) {
  switch (status) {
    case UART_ERROR_NONE:
      break;
    case UART_ERROR_TIMEOUT:
      Serial.println("UART: Timed out!");
      break;
    case UART_ERROR_NO_ANSWER:
      Serial.println("UART: Received nothing!");
      break;
    case UART_ERROR_NO_START:
      Serial.println("UART: Invalid incomming data (no start char)");
      break;
    case UART_ERROR_MESSAGE_TOO_LONG:
      Serial.println("UART: Invalid incomming data (buffer busted, no end chars)");
      break;
    default:
      Serial.println("UART: Unknown status number!");
      break;
  }
}



#endif