#ifndef __UART_SLAVE__
#define __UART_SLAVE__

#define PIN_RX 17
#define PIN_TX 16

#define UART_REQUEST            "Get"
#define UART_TIMEOUT_MS         2000
#define UART_BAUD_RATE          9600
#define UART_END_CHAR           '\n'
#define UART_START_CHAR         '\1'
#define UART_REQUEST_GAP_MS     20 // How long you wait before you can request again.
#define UART_MESSAGE_MAX_LENGTH 5

#define UART_ERROR_NONE             0
#define UART_ERROR_TIMEOUT          1
#define UART_ERROR_MESSAGE_TOO_LONG 2
#define UART_ERROR_NO_START         3
#define UART_ERROR_NO_ANSWER        4
#define UART_ERROR_INVALID_REQUEST  5

HardwareSerial UART(2);
String rx_buffer = "";

typedef struct UARTData {
  float temperature;        // Celcius
  float humidity;           // Percentage
  float wind_speed;         // m/s
  float wind_direction;     // degrees
  float pressure;           // kPA
  float light;              // Lux
  float precipitation;      // No idea
  int error;
} UARTData;

String format_weather(UARTData *dest)
{
  char buffer[256];

  snprintf(
    buffer,
    sizeof(buffer),
    "%cT: %.2f C, H: %.2f %%, L: %.2f Lx, W: %.2f m/s, Dir: %.2f rad, P: %.2f kPa, R: %.2f%c",
    UART_START_CHAR,
    dest->temperature,
    dest->humidity,
    dest->light,
    dest->wind_speed,
    dest->wind_direction,
    dest->pressure,
    dest->precipitation,
    UART_END_CHAR
  );

  return String(buffer);
}

int uart_slave_setup() {
  UART.begin(UART_BAUD_RATE, SERIAL_8N1, PIN_RX, PIN_TX);
  return UART_REQUEST_GAP_MS;
}

void uart_slave_answer(UARTData *dest) {
  while (UART.available()) {
    char received = UART.read();

    if (received == UART_END_CHAR) {
      rx_buffer.trim();

      Serial.print("Full message: ");
      Serial.println(rx_buffer);

      if (rx_buffer == UART_REQUEST) {
        UART.println(format_weather(dest));
        Serial.println("UART: Weather sent to base station");
      } else {
        dest->error = UART_ERROR_INVALID_REQUEST;
      }

      rx_buffer = "";
    } else {

      rx_buffer += received;

      if (rx_buffer.length() > UART_MESSAGE_MAX_LENGTH) {
          dest->error = UART_ERROR_MESSAGE_TOO_LONG;
          rx_buffer = "";
      }
    }
  }
}

void uart_err_mgr(int status) {
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
    case UART_ERROR_INVALID_REQUEST:
      Serial.println("UART: Received invalid incomming request");
      break;
    default:
      Serial.println("UART: Unknown status number!");
      break;
  }
}

#endif // __UART_SLAVE__
