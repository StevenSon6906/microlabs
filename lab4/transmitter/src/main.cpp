#include <Arduino.h>
#define MIC_PIN A1
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x3C, 0x8A, 0x1F, 0xA4, 0x1B, 0x58};

typedef struct {
  uint8_t sample;
} AudioPacket;

AudioPacket audio;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;      
  peerInfo.encrypt = false;   

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Peer added");
}

void loop() {
  int raw = analogRead(MIC_PIN);
  audio.sample = map(raw, 0, 4095, 0, 255);
  Serial.print("Sending sample: ");
  Serial.println(audio.sample);
  esp_now_send(broadcastAddress, (uint8_t*)&audio, sizeof(audio));
  delayMicroseconds(125);
}
