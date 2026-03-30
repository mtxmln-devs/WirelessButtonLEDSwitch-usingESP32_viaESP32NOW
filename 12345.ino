#include <esp_now.h>
#include <WiFi.h>
// --- CONFIGURATION ---
// BOARD 1 MAC: B0:CB:D8:8A:69:E8 
// BOARD 2 MAC: E0:8C:FE:30:A8:9C
// --- CONFIGURATION ---
// Change this to the OTHER board's MAC address
uint8_t broadcastAddress[] = {0xE0, 0x8C, 0xFE, 0x30, 0xA8, 0x9C}; 

const int buttonPin = 22; 
const int ledPin = 23;    

typedef struct struct_message {
    bool ledStatus;
} struct_message;

struct_message sendData;
struct_message recvData;

esp_now_peer_info_t peerInfo;

void OnDataSent(const esp_now_send_info_t *send_info, esp_now_send_status_t status) {
  // Feedback handled in loop
}

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&recvData, incomingData, sizeof(recvData));
  // Physical LED follows the data exactly
  digitalWrite(ledPin, recvData.ledStatus); 
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

// This tracks what we told the other board last.
// If it was HIGH (Released), and now it's LOW (Pressed), we send an update.
bool lastPhysicalState = HIGH; 

void loop() {
  bool currentPhysicalState = digitalRead(buttonPin);

  // If the button moves at all (Pressed OR Released)
  if (currentPhysicalState != lastPhysicalState) {
    
    // If the pin is LOW, the button is physically DOWN. Send 'true'.
    // If the pin is HIGH, the button is physically UP. Send 'false'.
    sendData.ledStatus = (currentPhysicalState == LOW); 
    
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendData, sizeof(sendData));
    
    if (result == ESP_OK) {
      Serial.println(sendData.ledStatus ? "PUSH: Sending ON" : "RELEASE: Sending OFF");
    }

    // Update our tracker
    lastPhysicalState = currentPhysicalState;
    
    // 50ms debounce is crucial for momentary buttons to prevent "chatter"
    delay(50); 
  }
}