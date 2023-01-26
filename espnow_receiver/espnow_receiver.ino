/*
  ESP-NOW Demo - Receive
  esp-now-demo-rcv.ino
  Reads data from Initiator
  
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <string>

// Define a data structure
typedef struct {
    String detectorID;
    char mac_address[18];  // 6 bytes in hexadecimal notation (2 characters per byte) plus 5 colons
    int8_t signal_strength;
} device_info_t;

// Create a structured object
device_info_t BTdevice;


// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&BTdevice, incomingData, sizeof(BTdevice));
  
  Serial.println("######DATA RECEIVED #######");
  Serial.print("DeviceID: ");
  Serial.println(BTdevice.detectorID); 
  Serial.print("Mac Address: ");
  Serial.println(BTdevice.mac_address);
  Serial.print("Signal Strength: ");
  Serial.println(BTdevice.signal_strength);
  Serial.println("");
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}