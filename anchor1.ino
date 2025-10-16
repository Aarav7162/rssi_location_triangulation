#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASS "YourWiFiPassword"
#define TARGET_SSID "tagESP"

#define BLYNK_AUTH "ZoFyLt0hTo429oaxvlmIcqsTp9Fxd1sI"

#define RSSI_VPIN V0

BlynkTimer timer;

void sendRSSI() {
  int n = WiFi.scanNetworks();
  int rssi = -100;

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == TARGET_SSID) {
      rssi = WiFi.RSSI(i);
      break;
    }
  }

  Serial.print("RSSI to ");
  Serial.print(TARGET_SSID);
  Serial.print(": ");
  Serial.println(rssi);

  Blynk.virtualWrite(RSSI_VPIN, rssi);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS);
  timer.setInterval(500, sendRSSI);
}

void loop() {
  Blynk.run();
  timer.run();
}
