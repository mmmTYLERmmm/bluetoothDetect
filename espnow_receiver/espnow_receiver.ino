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
#include "time.h"

// Contants used to access network time
const char* ssid     = "WIFI-SSID";
const char* password = "WIFI-PASSWORD";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 3600;
const int   maximumAttempts = 25;
bool        timeAcquired = false;

// Define a data structure
typedef struct {
    String detectorID;
    unsigned long time_string;
    char mac_address[18];  // 6 bytes in hexadecimal notation (2 characters per byte) plus 5 colons
    int8_t signal_strength;
} device_info_t;

// Create a structured object
device_info_t BTdevice;

// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&BTdevice, incomingData, sizeof(BTdevice));
  
  Serial.println("######DATA RECEIVED #######");
  if (timeAcquired) {
    Serial.print("Received Time: ");  
    printLocalTime();
  }
  Serial.print("Device ID: ");
  Serial.println(BTdevice.detectorID); 
  Serial.print("Device Time: "); 
  Serial.println(BTdevice.time_string);
  Serial.print("Mac Address: ");
  Serial.println(BTdevice.mac_address);
  Serial.print("Signal Strength: ");
  Serial.println(BTdevice.signal_strength);
  Serial.println("");
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  int attemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && attemptCount<maximumAttempts) {
    attemptCount = attemptCount+1;
    delay(500);
    Serial.print(">");
  }
  if (attemptCount < maximumAttempts){
    timeAcquired = true;
  }
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

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

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}