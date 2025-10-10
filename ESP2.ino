#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "RSSI Framework"
#define BLYNK_DEVICE_NAME "ESP2"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// Virtual pin for RSSI
#define RSSI_VPIN V1

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

  // --- Scan networks and find RSSI of target SSID ---
  int n = WiFi.scanNetworks();
  int rssiValue = -100; // default if not found

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == targetSSID) {
      rssiValue = WiFi.RSSI(i);
      break;
    }
  }

  // Send RSSI to Blynk
  Blynk.virtualWrite(RSSI_VPIN, rssiValue);
  Serial.print("RSSI for ");
  Serial.print(targetSSID);
  Serial.print(": ");
  Serial.println(rssiValue);

  delay(2000); // update every 2 seconds
}
