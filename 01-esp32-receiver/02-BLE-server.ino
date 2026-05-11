#ifndef __02_BLE_SERVER__
#define __02_BLE_SERVER__

#define SERVICE_UUID          "12345678-1234-1234-1234-1234567890ab"
#define CHAR_UUID             "abcd1234-1234-1234-1234-abcdef123456"
#define BLE_MTU               186

#define STATION_MAC_ADDRESS   "c4:de:e2:c0:0e:86"

#define BLE_BASE_STATION_NAME "Base station"

#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLERemoteCharacteristic.h>
#include <BLEUtils.h>

// Correct BLEAddress initialization
static BLEAddress sensorStationAddress(STATION_MAC_ADDRESS);

BLEClient* pClient = nullptr;
BLERemoteCharacteristic* pRemoteChar = nullptr;
bool state = false;

class WeatherCallback : public BLEClientCallbacks {

    void onConnect(BLEClient* pClient) override {
        Serial.println("BLE: Connected callback");
    }

    void onDisconnect(BLEClient* pClient) override {
        Serial.println("BLE: Disconnected callback");
    }
};


static void notifyCallback(
    BLERemoteCharacteristic* pChar,
    uint8_t* data,
    size_t length,
    bool isNotify)
{
    String message = "";

    for (size_t i = 0; i < length; i++) {
        message += (char)data[i];
    }

    Serial.println("BLE Received: " + message);
    state = !state;
    digitalWrite(19, state);
}

int ble_server_setup() {
    pinMode(19, OUTPUT);

    Serial.println("BLE: Setting up...");
    BLEDevice::setMTU(BLE_MTU);
    BLEDevice::init(BLE_BASE_STATION_NAME);

    Serial.print("- Client MAC: ");
    Serial.println(BLEDevice::getAddress().toString().c_str());

    Serial.print("- Server MAC: ");
    Serial.println(STATION_MAC_ADDRESS);

    // Client setup
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new WeatherCallback());

    Serial.println("BLE: Connecting...");

    // Connect directly using MAC address
    if (!pClient->connect(sensorStationAddress)) {
        Serial.println("BLE ERROR: Failed to connect");
        Serial.println("YOU NEED TO RESTART THE ESP32 TO FIX THIS ISSUE");
        return -1;
    }

    uint16_t mtu = pClient->getMTU();
    Serial.println("Default MTU: " + String(mtu));

    pClient->setMTU(BLE_MTU);
    delay(200);
    mtu = pClient->getMTU();

    Serial.println("Negotiated MTU: " + String(mtu));
    if (mtu != BLE_MTU) {
        Serial.println("BLE ERROR: MTU did not negociate, you're stuck at 20. Not enough for the full message.");
    }

    Serial.println("BLE: Connected!");

    // Get remote service
    BLERemoteService* pService = pClient->getService(SERVICE_UUID);

    if (pService == nullptr) {
      Serial.println("BLE ERROR: Service not found");
      Serial.println("BLE: Disconnecting...");
      pClient->disconnect();
      Serial.println("YOU NEED TO RESTART THE ESP32 TO FIX THIS ISSUE");
      return -1;
    }

    Serial.println("BLE: Service found");

    // Get remote characteristic
    pRemoteChar = pService->getCharacteristic(CHAR_UUID);

    if (pRemoteChar == nullptr) {
        Serial.println("BLE ERROR: Characteristic not found");
        Serial.println("BLE: Disconnecting...");
        pClient->disconnect();
        Serial.println("YOU NEED TO RESTART THE ESP32 TO FIX THIS ISSUE");
        return -1;
    }

    Serial.println("BLE: Characteristic found");

    // Register for notifications
    if (pRemoteChar->canNotify()) {
        pRemoteChar->registerForNotify(notifyCallback);
        Serial.println("BLE: Notifications enabled");
    } else {
        Serial.println("BLE ERROR: Characteristic cannot notify");
        Serial.println("THE WEATHER STATION IS WRONG!");
        return -1;
    }

    return 0;
}

#endif