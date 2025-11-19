#define BLYNK_TEMPLATE_ID "ID"
#define BLYNK_TEMPLATE_NAME "RSSI Location Triangulation"
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASS"
#define TARGET_SSID "tagESP"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>

#define BLYNK_AUTH "YOUR_BLYNK_AUTH"
#define RSSI_VPIN V0
#define AUTO_ANGLE_VPIN 201

BlynkTimer timer;

Servo pan;
Servo tilt;

int panPos = 90;
int tiltPos = 90;

enum Mode { MODE_MANUAL, MODE_AUTO };
Mode currentMode = MODE_MANUAL;

int autoAngleRaw = 0;

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

int pullAngleFromCloud() {
  if (WiFi.status() != WL_CONNECTED) return autoAngleRaw;
  HTTPClient http;
  String url = "http://blynk.cloud/external/api/get?token=" + 
               String(BLYNK_AUTH) + "&V" + String(AUTO_ANGLE_VPIN);
  WiFiClient client;
  http.begin(client, url);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    int val = payload.toInt();
    http.end();
    return val;
  }
  http.end();
  return autoAngleRaw;
}

void updateAutoMode() {
  int cloudVal = pullAngleFromCloud();
  if (cloudVal != autoAngleRaw) {
    autoAngleRaw = cloudVal;
    Serial.printf("[API] Fetched V201 = %d\n", autoAngleRaw);
  }
  int angle = autoAngleRaw * 2;
  angle = constrain(angle, 0, 180);
  panPos = angle;
  pan.write(panPos);
  Serial.printf("[AUTO] PAN -> %d (raw %d ×2)\n", panPos, autoAngleRaw);
}

String extractNumbers(String s) {
  String out = "";
  for (char c : s) {
    if (isDigit(c) || c == ',') out += c;
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
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS);
  timer.setInterval(2500L, sendRSSI);
  timer.setInterval(1000L, []() {
    if (currentMode == MODE_AUTO) updateAutoMode();
  });
  Serial.println("System Ready.");
}

void loop() {
  Blynk.run();
  timer.run();
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "MANUAL") {
    currentMode = MODE_MANUAL;
    Serial.println("MODE → MANUAL");
    return;
  }
  if (cmd == "AUTO") {
    currentMode = MODE_AUTO;
    Serial.println("MODE → AUTO");
    return;
  }
  if (currentMode == MODE_AUTO) {
    Serial.println("AUTO MODE ACTIVE → Manual commands blocked.");
    return;
  }

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
