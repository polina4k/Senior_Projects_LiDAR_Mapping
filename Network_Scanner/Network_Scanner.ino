
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Initializing WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
}

void loop() {
  Serial.println("Starting WiFi scan...");

  int n = WiFi.scanNetworks();

  if (n == WIFI_SCAN_FAILED) {
    Serial.println("WiFi scan failed!");
  }
  else if (n == 0) {
    Serial.println("No networks found.");
  }
  else {
    Serial.print("Networks found: ");
    Serial.println(n);

    for (int i = 0; i < n; i++) {
      Serial.print("SSID: ");
      Serial.print(WiFi.SSID(i));

      Serial.print(" | RSSI: ");
      Serial.print(WiFi.RSSI(i));

      Serial.print(" dBm | Channel: ");
      Serial.println(WiFi.channel(i));
    }
  }

  WiFi.scanDelete();

  Serial.println("--------------------");
  delay(5000);
}
