#include <BLEDevice.h>

static BLEAddress *serverAddress;
static BLERemoteCharacteristic* remoteCharacteristic;

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

bool doConnect = false;
bool connected = false;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

  Serial.print("Received: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)pData[i]);
  }

  Serial.println();
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {

    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {

      Serial.println("Found server!");

      serverAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;

      BLEDevice::getScan()->stop();
    }
  }
};

bool connectToServer() {

  BLEClient* pClient = BLEDevice::createClient();

  if (!pClient->connect(*serverAddress)) {
    return false;
  }

  BLERemoteService* pRemoteService =
      pClient->getService(SERVICE_UUID);

  if (pRemoteService == nullptr) {
    return false;
  }

  remoteCharacteristic =
      pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);

  if (remoteCharacteristic == nullptr) {
    return false;
  }

  remoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}

void setup() {
  Serial.begin(115200);

  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();

  pBLEScan->setAdvertisedDeviceCallbacks(
      new MyAdvertisedDeviceCallbacks());

  pBLEScan->setActiveScan(true);
  pBLEScan->start(0);

  Serial.println("Scanning...");
}

void loop() {

  if (doConnect && !connected) {

    if (connectToServer()) {
      Serial.println("Connected to server");
    } else {
      Serial.println("Failed to connect");
    }

    doConnect = false;
  }

  delay(1000);
}