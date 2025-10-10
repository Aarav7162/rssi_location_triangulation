#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "RSSI Framework"
#define BLYNK_DEVICE_NAME "ESP1"
#define BLYNK_AUTH_TOKEN "auth"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "ssid";
char pass[] = "pass";

#define RSSI_VPIN V0

const char* targetSSID = "SomeGuyWithPretzels";

void setup() {
  Serial.begin(9600);
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  int n = WiFi.scanNetworks();
  int rssiValue = 0;

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == targetSSID) {
      rssiValue = WiFi.RSSI(i);
      break;
    }
  }

  Blynk.virtualWrite(RSSI_VPIN, rssiValue);
  Serial.print("RSSI for ");
  Serial.print(targetSSID);
  Serial.print(": ");
  Serial.println(rssiValue);

  delay(2000);
}
