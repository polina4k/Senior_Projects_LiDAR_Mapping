
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// Receiver's MAC address:  1C:C3:AB:2A:19:FC
uint8_t receiverMAC[] = {
  0x1C, 0xC3, 0xAB, 0x2A, 0x19, 0xFC
};

typedef struct {
  uint32_t sequence;
  float angle;
  float distance;
} LidarPacket;

const unsigned long interval = 2500;
unsigned long previousTime = 0;

uint32_t packetCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 1;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver");
    return;
  }

  Serial.println("Transmitter ready!");
}

void loop() {
  unsigned long currentTime = micros();

  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;

    LidarPacket packet;

    packet.sequence = packetCount++;
    packet.angle = (packet.sequence * 10) % 360;
    packet.distance = 1000.0 + random(0, 2000);

    esp_err_t result = esp_now_send(
      receiverMAC,
      (uint8_t*)&packet,
      sizeof(packet)
    );

    if (result != ESP_OK) {
      Serial.println("Send failed");
    }
  }
}