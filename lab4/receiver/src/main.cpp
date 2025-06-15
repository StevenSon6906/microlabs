#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include <ringbuffer.hpp>
typedef struct { uint8_t sample; } AudioPacket;

static constexpr uint32_t SAMPLE_RATE = 8000;
static constexpr uint32_t SAMPLE_PERIOD = 1000000UL / SAMPLE_RATE;
const int TIP = 25;
const int RING1 = 26;

jnk0le::Ringbuffer<uint8_t, 8192> audioBuf;
void onReceive(const uint8_t* mac_addr, const uint8_t* data, int len) {
  Serial.printf("Got ESP-NOW packet, len=%d. First byte: %02X\n",
                len, len>0 ? data[0] : 0);

  for (int i = 0; i < len; i++) {
    uint8_t sample = data[i];
    Serial.printf("Sample %d: %02X\n", i, sample);
    for (int i = 0; i < len; i++) {
       audioBuf.insert(data[i]);
    }

  }
}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
    pinMode(TIP, OUTPUT);
  pinMode(RING1, OUTPUT);
  digitalWrite(TIP, LOW);
  digitalWrite(RING1, LOW);

} 

void loop() {
    static uint32_t next = micros();
    while ((int32_t)(micros() - next) < 0) {}
    next += SAMPLE_PERIOD;
    uint8_t sample = 128;
    if (!audioBuf.isEmpty()) {
        audioBuf.remove(sample);
    }
    dacWrite(TIP, sample);
    dacWrite(RING1, sample);
}