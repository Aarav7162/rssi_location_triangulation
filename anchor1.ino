#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "RSSI Location Triangulation"
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
#define TARGET_SSID "tagESP"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

#define BLYNK_AUTH "YOUR_AUTH_TOKEN"
#define RSSI_VPIN V0

BlynkTimer timer;

Servo pan;
Servo tilt;

int panPos = 90;
int tiltPos = 90;

void sendRSSI() {
  int n = WiFi.scanNetworks(false, true);
  int rssi = -100;

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == TARGET_SSID) {
      rssi = WiFi.RSSI(i);
      break;
    }
  }

  Blynk.virtualWrite(RSSI_VPIN, rssi);
  Serial.print("RSSI → ");
  Serial.println(rssi);
}

String extractNumbers(String s) {
  String out = "";
  for (char c : s) {
    if (isDigit(c) || c == ',' ) out += c;
  }
  return out;
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pan.attach(D1);
  tilt.attach(D5);

  pan.write(panPos);
  tilt.write(tiltPos);

  Serial.println("Pan/Tilt Ready.");
  Serial.println("Accepted formats:");
  Serial.println("(60,70)");
  Serial.println("60,70");
  Serial.println("PAN 60 TILT 70");
  Serial.println("120  -> PAN only");
  Serial.println("T80  -> TILT only");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS);

  timer.setInterval(2500L, sendRSSI);
}

void loop() {
  Blynk.run();
  timer.run();

  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.indexOf(',') != -1) {
    String nums = extractNumbers(cmd);
    int commaIdx = nums.indexOf(',');
    if (commaIdx > 0) {
      int x = nums.substring(0, commaIdx).toInt();
      int y = nums.substring(commaIdx + 1).toInt();

      x = constrain(x, 0, 180);
      y = constrain(y, 0, 180);

      panPos = x;
      tiltPos = y;

      pan.write(panPos);
      tilt.write(tiltPos);

      Serial.printf("PAN → %d   TILT → %d\n", panPos, tiltPos);
      return;
    }
  }

  if (cmd.startsWith("PAN") || cmd.startsWith("P")) {
    int val = cmd.substring(cmd.indexOf("N") + 1).toInt();
    val = constrain(val, 0, 180);
    panPos = val;
    pan.write(panPos);
    Serial.printf("PAN → %d\n", panPos);
    return;
  }

  if (cmd.startsWith("TILT") || cmd.startsWith("T")) {
    int val = cmd.substring(cmd.indexOf("T") + 1).toInt();
    val = constrain(val, 0, 180);
    tiltPos = val;
    tilt.write(tiltPos);
    Serial.printf("TILT → %d\n", tiltPos);
    return;
  }

  if (cmd.length() <= 3 && isDigit(cmd[0])) {
    int val = cmd.toInt();
    val = constrain(val, 0, 180);
    panPos = val;
    pan.write(panPos);
    Serial.printf("PAN → %d\n", panPos);
    return;
  }

  Serial.println("Invalid command.");
}
