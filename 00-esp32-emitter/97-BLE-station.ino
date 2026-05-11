#ifndef __02_BLE_SERVER__
#define __02_BLE_SERVER__

#define SERVICE_UUID          "12345678-1234-1234-1234-1234567890ab"
#define CHAR_UUID             "abcd1234-1234-1234-1234-abcdef123456"

#define SERVER_MAC_ADDRESS    "c0:49:ef:4b:20:a2"

#define BLE_BASE_STATION_NAME "Weather station"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "98-UART-slave.ino"

BLECharacteristic* pCharacteristic = nullptr;

bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("BLE: Client connected");
  }

  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("BLE: Client disconnected");
    // Restart advertising so clients can reconnect
    BLEDevice::startAdvertising();
    Serial.println("BLE: Advertising restarted");
  }
};

int ble_station_setup() {
    BLEDevice::init(BLE_BASE_STATION_NAME);

    Serial.print("Station MAC: ");
    Serial.println(BLEDevice::getAddress().toString().c_str());

    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    BLEService* pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
      CHAR_UUID,
      BLECharacteristic::PROPERTY_NOTIFY |
      BLECharacteristic::PROPERTY_READ
    );

    pCharacteristic->addDescriptor(new BLE2902());

    Serial.println("BLE: Starting BLE service");
    pService->start();
    Serial.println("BLE: Service started");

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();
    Serial.println("BLE: Advertising started");
    return 0;
}

void ble_notify(UARTData* dest) {
    if (!deviceConnected) {
      Serial.println("BLE: No device connected. No notifications sent.");
      return;
    }

    String to_send = format_weather(dest);

    pCharacteristic->setValue(to_send.c_str());
    pCharacteristic->notify();

    Serial.println("BLE Sent: " + to_send);
}

#endif