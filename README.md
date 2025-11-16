# RSSI Location Triangulation + Servo Control (Unified System)

# Overview
This project now integrates **indoor localization**, **RSSI fingerprinting**, **HTTP sniffing**, and a **pan/tilt servo controller** into a unified architecture.

**anchor1** has been upgraded: it still reports RSSI for `tagESP`, but it now also exposes a serial-based **pan/tilt servo command interface**. This allows you to manually control a camera/antenna turret using commands such as:
```
60,70        → PAN=60, TILT=70
PAN 120      → PAN=120
TILT 40      → TILT=40
```
All other anchors retain their previous roles. The main dashboard is hosted at the link below:

**Dashboard:** [https://rssi-dashboard.netlify.app](https://rssi-dashboard.netlify.app)

This integrated system is intended for education, IoT experimentation, wireless tracking research, servo‑based directional antennas, and general R&D prototyping.

---

# Folder Structure
```
├── anchor1.ino             # Reports RSSI to V0 + handles full pan/tilt servo control via Serial
├── anchor2.ino             # Reports RSSI to V1
├── anchor3.ino             # Reports RSSI to V2, sniffing engine, payload → V10
├── anchor4.ino             # Reports RSSI to V3
├── tag_esp.ino             # Soft‑AP SSID `tagESP` with small HTTP server
├── calibration.py          # Collects RSSI fingerprints for the grid
├── tracker.py              # Computes (x,y), pushes to Blynk, controls sniff/pause
├── fingerprint_data.json   # Stored RSSI fingerprints
└── README.md               # Documentation
```

---

# Quick Mapping
| Device       | Function                                                | Blynk Pin |
|--------------|---------------------------------------------------------|-----------|
| anchor1.ino  | Reports RSSI; **pan/tilt servo control via Serial**     | V0        |
| anchor2.ino  | Reports RSSI                                            | V1        |
| anchor3.ino  | Reports RSSI; sniffing engine; payload → V10           | V2 / V10  |
| anchor4.ino  | Reports RSSI                                            | V3        |
| tracker.py   | Computes (x,y) → Blynk                                  | X: V6, Y: V7 |
| tracker.py   | Watches V9 to pause RSSI and read sniff serial          | Trigger: V9 |

---

# Prerequisites (Windows)
1. Python 3.8+ installed.
2. Virtual environment recommended:
   ```
   python -m venv .venv
   .venv\Scripts\activate
   pip install --upgrade pip
   pip install requests numpy pyserial
   ```
3. Arduino IDE for flashing ESP8266.
4. Blynk template + token matching the ESP code.

---

# Configuration
Before flashing or running scripts, configure:
- **BLYNK_AUTH** – must match across all anchors + Python.
- **WIFI_SSID / WIFI_PASS** – network anchors connect to.
- **TARGET_SSID** – usually `tagESP`.
- **RSSI virtual pins** – ensure anchors map V0–V3.
- **anchor1** – now includes **servo pins** and serial parser.
- **tracker.py**:
  - SERIAL_PORT for anchor3
  - RSSI sample count and interval
  - calibration file path

---

# Calibration
Same process as before:
1. Keep anchors fixed.
2. Move tag ESP across grid coordinates.
3. Run:
   ```
   python calibration.py
   ```
4. Script collects and averages RSSI from anchors.
5. Results stored in `fingerprint_data.json`.

### Fingerprint JSON Format
```json
{
    "1.0_2.0": { "V0": -63, "V1": -40, "V2": -55, "V3": -60 },
    "2.0_3.0": { "V0": -70, "V1": -48, "V2": -59, "V3": -65 }
}
```

---

# Running the Tracker
1. Ensure `fingerprint_data.json` exists.
2. Configure constants.
3. Run:
   ```
   python tracker.py
   ```

### Live Behavior
- Continuously collects RSSI → computes nearest fingerprint → sends (x,y) to V6/V7.
- When V9 = 1:
  - RSSI sampling pauses.
  - Opens serial to **anchor3**.
  - Streams sniff payloads.
  - When V9 returns to 0: resumes RSSI.

---

# Servo Control (anchor1)
anchor1 now includes a **full serial command interface** for pan/tilt control.

### Accepted Formats
```
60,70           # paired coordinates
(60,70)         # parentheses OK
PAN 120         # single-axis
TILT 40
120             # PAN only
T60             # TILT only
(PAN,TILT)=(90,45)
```

### Behavior
- All commands instantly move servos.
- Positions clamped 0–180.
- RSSI reporting unaffected.
- Servos remain at their last commanded angle until new input.

This enables directional antenna pointing, camera aiming, or robotics testing.

---

# Dashboard Controls
- **Track:** updates RSSI values + coordinates.
- **Lock On:** cutscene (visual only).
- **Launch:** triggers V41 for external servo.
- **RSSI Nodes:** updates V0–V3.
- **Sniff Trigger (V9):** pauses tracking and activates sniff read mode.

---

# Sniffing & Trilateration
The system performs two parallel tasks:

### 1. **Fingerprint-based Trilateration**
tracker.py:
- Collects RSSI vector `[V0, V1, V2, V3]`.
- Compares to fingerprints.
- Finds nearest match using Euclidean distance.
- Sends coordinates to Blynk.

### 2. **HTTP Sniffing (anchor3)**
When V9 = 1:
- anchor3 attempts connection to tag soft-AP.
- Repeatedly requests `http://192.168.4.56/`.
- Streams payloads via Serial.
- tracker.py prints them live.

---

# Troubleshooting
- **No RSSI updates:** check WiFi + Blynk token.
- **Zero-value anchors:** reboot stuck anchor.
- **No servo motion:** check D1 and D5 wiring; ensure power.
- **Python serial errors:** verify COM port.
- **Sniff mode does nothing:** ensure tagESP is powered and broadcasting.

---

# Ethics & Safety
Use sniffing only on networks/devices you own.
The brute-force component in anchor3 is experimental and must not be used on arbitrary networks.

---

# Developer Notes
- anchor1 now merges both **RSSI scanning** and **servo motor control**.
- Servos operate fully independently from RSSI tasks.
- RSSI + servo processing can run concurrently due to lightweight logic.
- anchor3 still handles the heavy sniffing workload.
- tracker.py remains the system orchestrator.

---

# End of README
This documentation now reflects the unified system architecture with servo‑enabled anchor1.

