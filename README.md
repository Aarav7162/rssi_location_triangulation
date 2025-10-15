# ESP Wi-Fi RSSI Position Tracking System

## Overview
This project implements a Wi-Fi-based positioning and tracking system using multiple ESP8266/ESP32 nodes and a single transmitting tag device. The system estimates the angle and distance of the tracked tag from multiple fixed points using RSSI fingerprinting and triangulation. The data is then visualized and processed using a Python dashboard.

Future versions will include a Lego turret that physically tracks the tag using servos for automated aiming or pointing systems.

---

## Core Concept
The ESP nodes measure the **RSSI (Received Signal Strength Indicator)** of a moving Wi-Fi tag (named `tagESP`). Each ESP sends its readings to **Blynk Cloud**, from where a Python program fetches the RSSI data, calculates the approximate angle and distance and visualizes or triggers movements accordingly.

This setup can be used for:
- Indoor position tracking
- Object following robots
- BLE/Wi-Fi hybrid localization systems
- Educational IoT research and demos

---

## Hardware Setup
| Device | Purpose | Quantity |
|---------|----------|-----------|
| ESP8266 (NodeMCU) | Fixed nodes to read RSSI | 4 |
| ESP8266 / ESP32 | Transmitting tag device | 1 |
| Ultrasonic sensor (HC-SR04) | Distance estimation (future) | 1 |
| Servo motor | Direction control (future turret) | 2 |
| Breadboard, jumper wires, power cables | Connectivity | - |

---

## Node (Receiver) Code Summary
Each ESP8266 connects to Blynk and reads the RSSI of a Wi-Fi SSID named `tagESP`. It transmits this RSSI data every few seconds to Blynk Cloud.

```cpp
#define BLYNK_TEMPLATE_ID "TMPL3j_L7mmM3"
#define BLYNK_TEMPLATE_NAME "RSSI Location Triangulation"
#define BLYNK_DEVICE_NAME "RSSI_ESP"
#define BLYNK_AUTH_TOKEN "ZoFyLt0hTo429oaxvlmIcqsTp9Fxd1sI"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

String targetNetwork = "tagESP";
int vpin = V0; // Change for each ESP node (V0, V1, V2, V3)

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  int rssi = WiFi.RSSI();
  Blynk.virtualWrite(vpin, rssi);
  delay(1000);
}
```

---

## Tag (Emitter) Code Summary
The emitter ESP simply creates a Wi-Fi Access Point named `tagESP`. The nodes detect and measure its signal.

```cpp
#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.softAP("tagESP", "12345678");
  Serial.println("Tag active: tagESP");
}

void loop() {
  delay(1000);
}
```

---

## Python Dashboard
The Python script fetches RSSI values from Blynk Cloud using its REST API, averages them for 20 seconds (calibration), and estimates the tag's **angle** relative to the node grid.

### Requirements
```bash
pip install requests numpy matplotlib
```

### Features
- RSSI averaging and filtering
- Real-time coordinate calculation
- JSON saving of calibration and live data
- Placeholder for servo motion and ultrasonic integration

### Example Output
```
Starting calibration (20s)...
Average RSSI Node1=-43, Node2=-51, Node3=-47, Node4=-50
Estimated Angle: 62°
Coordinates: (1.84m, 2.1m)
```

---

## Data Structure (JSON)
Data is saved automatically to `data.json`:

```json
{
  "calibration": {
    "node1": -45,
    "node2": -52,
    "node3": -48,
    "node4": -50
  },
  "live": {
    "angle": 61.5,
    "x": 1.84,
    "y": 2.10
  }
}
```

---

## Lego Turret (Future Addition)
Once position tracking is stable, the project will expand into a **motorized Lego turret**:
- Controlled via ESP8266 + PCA9685 servo driver
- Tracks the tag in real-time
- Rotates based on calculated angle
- Fires a laser or LED at the tracked position

---

## Future Roadmap
- [ ] Integrate live servo rotation
- [ ] Add ultrasonic-based distance calculation
- [ ] Build Lego turret base
- [ ] Use Kalman filter for smoother motion
- [ ] Add GUI visualization with Tkinter or PyQt

---

## References
- Blynk IoT Cloud API
- ESP8266 Wi-Fi documentation
- Signal triangulation and localization research
- BLE AoA/AoD literature (for future BLE integration)

---

## Author
**Aarav** — Co-Founder of NeuroCode, developer of **Synk IDE** and **Wi-Fi Positioning System**.
