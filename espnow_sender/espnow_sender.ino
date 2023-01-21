

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include "HardwareSerial.h"

const int LED_BUILTIN = 2;
const int CUTOFF = -50;

HardwareSerial serial(0);

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xC0, 0x49, 0xEF, 0xE6, 0x13, 0x04}; // board B

// Define a data structure
typedef struct {
    unsigned long time_string;
    char mac_address[18];  // 6 bytes in hexadecimal notation (2 characters per byte) plus 5 colons
    int8_t signal_strength;
} device_info_t;

// Create a structured object
device_info_t BTdevice;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  BLEDevice::init("");
  serial.println("Connecting to server...");
  
  // Set up Serial Monitor
  Serial.begin(115200);
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  BLEScanResults results = scan->start(1);
  int best = CUTOFF;
  
  // for each device found we will print if signal is strong enough i.e. rssi > cutoff
  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice device = results.getDevice(i);
    int rssi = device.getRSSI();
//    if (rssi > best) {
//      best = rssi;
//    }
    if (rssi > CUTOFF) {
       // Format structured data
      BTdevice.time_string = millis();
      strcpy(BTdevice.mac_address, device.getAddress().toString().c_str());
      BTdevice.signal_strength = rssi;

      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &BTdevice, sizeof(BTdevice));

      }
    }
  }
