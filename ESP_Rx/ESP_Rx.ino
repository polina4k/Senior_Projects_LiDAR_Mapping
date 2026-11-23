
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

typedef struct {
  uint32_t sequence;
  float angle;
  float distance;
} LidarPacket;

portMUX_TYPE statsMux = portMUX_INITIALIZER_UNLOCKED;

uint32_t lastSequence = 0;
uint32_t receivedPackets = 0;
uint32_t lostPackets = 0;
bool firstPacket = true;



void onReceive(const esp_now_recv_info_t *info,
               const uint8_t *data,
               int len) {

  if (len != sizeof(LidarPacket)) {
    return;
  }

  LidarPacket packet;
  memcpy(&packet, data, sizeof(packet));

  portENTER_CRITICAL(&statsMux);

  receivedPackets++;

  if (!firstPacket && packet.sequence > lastSequence + 1) {
    lostPackets += packet.sequence - lastSequence - 1;
  }

  firstPacket = false;
  lastSequence = packet.sequence;

  portEXIT_CRITICAL(&statsMux);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize Wi-Fi in station mode
  WiFi.mode(WIFI_STA);
  delay(500);

  // Read MAC address directly from Wi-Fi driver
  uint8_t mac[6];

  esp_err_t macResult = esp_wifi_get_mac(WIFI_IF_STA, mac);

  if (macResult == ESP_OK) {
    Serial.printf(
      "Receiver MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
      mac[0], mac[1], mac[2],
      mac[3], mac[4], mac[5]
    );
  } else {
    Serial.printf("Failed to read MAC: %d\n", macResult);
    return;
  }

  // Configure ESP-NOW channel
  esp_err_t channelResult =
      esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (channelResult != ESP_OK) {
    Serial.printf("Failed to set channel: %d\n", channelResult);
    return;
  }

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  Serial.println("Receiver ready!");
}


void loop() {
  static unsigned long previousTime = 0;
  static uint32_t previousReceived = 0;

  unsigned long currentTime = millis();

  if (currentTime - previousTime >= 1000) {
    unsigned long elapsed = currentTime - previousTime;
    previousTime = currentTime;

    uint32_t received;
    uint32_t lost;

    portENTER_CRITICAL(&statsMux);
    received = receivedPackets;
    lost = lostPackets;
    portEXIT_CRITICAL(&statsMux);

    uint32_t packetsThisInterval =
        received - previousReceived;

    previousReceived = received;

    float packetRate =
        packetsThisInterval * 1000.0 / elapsed;

    float lossPercent =
        (received + lost > 0)
        ? 100.0 * lost / (received + lost)
        : 0.0;

    Serial.printf(
      "Rate: %.1f pkt/s | Received: %lu | Lost: %lu | Loss: %.2f%%\n",
      packetRate,
      (unsigned long)received,
      (unsigned long)lost,
      lossPercent
    );
  }
}
