#include <WiFi.h>

const char* ssid = "Palina’s iPhone";
const char* password = "noguests=";


void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to hotspot...");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected successfully!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nConnection failed.");
  }
  

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Hotspot connection active");
  } else {
    Serial.println("Hotspot disconnected");
  }

  delay(2000);
}
