#ifndef __02_BLE_SERVER__
#define __02_BLE_SERVER__

#define SERVICE_UUID          "12345678-1234-1234-1234-1234567890ab"
#define CHAR_UUID             "abcd1234-1234-1234-1234-abcdef123456"
#define STATION_MAC_ADDRESS   "c4:de:e2:c0:0e:86"
#define BLE_BASE_STATION_NAME "Base station" 

#include <BLEDevice.h>
#include <BLEClient.h>
#include <BLERemoteCharacteristic.h>
#include <BLEUtils.h>

BLEAddress sensorStationAddress("");
BLERemoteCharacteristic* pRemoteChar;

class WeatherCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pClient) {
      Serial.println("onConnect called");
    }
    void onDisconnect(BLEClient* pClient) {
      Serial.println("onConnect called");
    }
};

static void notifyCallback(
    BLERemoteCharacteristic* pChar,
    uint8_t* data,
    size_t length,
    bool isNotify)
{
    String message = "";

    for (int i = 0; i < length; i++) {
        message += (char)data[i];
    }

    Serial.println("BLE Received: " + message);
}

int ble_server_setup() {
  BLEDevice::init(BLE_BASE_STATION_NAME);
  BLEClient* pClient = BLEDevice::createClient();

  Serial.print("Server MAC: ");
  Serial.println(BLEDevice::getAddress().toString().c_str());

  Serial.print("Station MAC:");
  Serial.println(STATION_MAC_ADDRESS);

  pClient->setClientCallbacks(new WeatherCallback());

  Serial.println("BLE: Connect called...");
  if (pClient->connect(sensorStationAddress)) {
    Serial.println("BLE: Connected!");
  } else {
    Serial.println("BLE ERROR: Failed to establish connection");
  }

  BLERemoteService* pService = pClient->getService(SERVICE_UUID);
  pRemoteChar = pService->getCharacteristic(CHAR_UUID);

  if (pRemoteChar->canNotify()) {
    Serial.println("BLE: can notify, callback binded. (does not mean connected to station)");
    pRemoteChar->registerForNotify(notifyCallback);
  } else {
    Serial.println("BLE: Not registered for notifications...");
  }

  return 0;
}

#endif