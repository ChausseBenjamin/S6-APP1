#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLERemoteCharacteristic.h>

// MAC address of the SERVER ESP32
static BLEAddress serverAddress("c4:de:e2:c0:0e:86");

// Must match the server
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

BLERemoteCharacteristic* remoteCharacteristic;
BLEClient* pClient;

// Called whenever the server sends a notification
static void notifyCallback(
    BLERemoteCharacteristic* pBLERemoteCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify) {

  Serial.print("Received: ");

  for (size_t i = 0; i < length; i++) {
    Serial.print((char)pData[i]);
  }

  Serial.println();
}

bool connectToServer() {

  Serial.println("Connecting to server...");

  pClient = BLEDevice::createClient();

  // Connect using known MAC address
  if (!pClient->connect(serverAddress)) {
    Serial.println("Connection failed");
    return false;
  }

  Serial.println("Connected");

  // Get service
  BLERemoteService* remoteService =
      pClient->getService(SERVICE_UUID);

  if (remoteService == nullptr) {
    Serial.println("Service not found");
    pClient->disconnect();
    return false;
  }

  // Get characteristic
  remoteCharacteristic =
      remoteService->getCharacteristic(CHARACTERISTIC_UUID);

  if (remoteCharacteristic == nullptr) {
    Serial.println("Characteristic not found");
    pClient->disconnect();
    return false;
  }

  // Enable notifications
  remoteCharacteristic->registerForNotify(notifyCallback);

  Serial.println("Notifications enabled");

  return true;
}

void setup() {

  Serial.begin(115200);

  BLEDevice::init("");

  connectToServer();
}

void loop() {

  // Reconnect if disconnected
  if (!pClient->isConnected()) {

    Serial.println("Disconnected. Reconnecting...");

    connectToServer();
  }

  delay(2000);
}