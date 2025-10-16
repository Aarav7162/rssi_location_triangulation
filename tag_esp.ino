#include <ESP8266WiFi.h>

#define EMIT_SSID "tagESP"
#define EMIT_PASS ""

void setup() {
  Serial.begin(115200);
  WiFi.softAP(EMIT_SSID, EMIT_PASS);
  Serial.print("Emitting SSID: ");
  Serial.println(EMIT_SSID);
}

void loop() {
  delay(1000);
}