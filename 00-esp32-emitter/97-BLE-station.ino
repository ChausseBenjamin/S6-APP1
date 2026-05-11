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


BLECharacteristic *pCharacteristic;

int ble_station_setup() {
  BLEDevice::init(BLE_BASE_STATION_NAME);
  
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_READ
  );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();
  return 0;
}

void ble_notify(UARTData *dest) {
  String to_send = format_weather(dest);

  pCharacteristic->setValue(to_send.c_str());
  pCharacteristic->notify();

  Serial.println("BLE Sent: " + to_send);
}

#endif