# RSSI Location Triangulation

# Overview
This folder contains a complete indoor localization/fingerprinting system built using ESP8266 anchors, a soft‑AP tag (`tagESP`), and Python utilities. The system is designed for estimating the (x,y) position of a mobile device using RSSI (Received Signal Strength Indicator) fingerprinting. It uses multiple fixed anchors to trilaterate the tag device's position (via 1‑NN matching on RSSI fingerprints) and - when triggered - can sniff and stream the tag's HTTP requests (the tag runs a tiny HTTP server at http://192.168.4.56/ to demonstrate interception of unsecure communication), allowing you to capture short payloads served by the tag. The main controls dashboard is at [Dashboard Website](https://rssi-dashboard.netlify.app), and a repo that explains more about it is at [Repo](https://github.com/Aarav7162/rssi_dashboard).

It is intended for educational purposes, proof-of-concept IoT localization, and controlled indoor tracking experiments.

## Folder Structure
```
├── anchor1.ino             # Reports RSSI (V0), manual servo mode, and Auto-Target servo mode using Blynk Cloud API (V201)
├── anchor2.ino             # ESP8266 sketch reporting RSSI to Blynk V1
├── anchor3.ino             # ESP8266 sketch reporting RSSI to Blynk V2, handles sniffing triggered by V9, returns payload to V10
├── anchor4.ino             # ESP8266 sketch reporting RSSI to Blynk V3
├── tag_esp.ino             # ESP soft-AP serving SSID `tagESP` with simple HTTP payload and WiFi beacon
├── calibration.py          # Python utility to collect RSSI fingerprints at known grid points
├── tracker.py              # Python live tracker: estimates position and updates Blynk V6/V7, handles serial streaming when V9 = 1
├── fingerprint_data.json   # JSON file storing collected RSSI fingerprints
└── README.md               # Documentation (this file)
```

## Quick Mapping
| Device       | Function                                         | Blynk Pin |
|--------------|--------------------------------------------------|-----------|
| anchor1.ino  | RSSI, pan/tilt manual control, auto-target (API)    | V0, V201        |
| anchor2.ino  | Reports RSSI to second anchor                    | V1        |
| anchor3.ino  | Reports RSSI, handles sniffing                   | V2 / V10  |
| anchor4.ino  | Reports RSSI to fourth anchor                    | V3        |
| tracker.py   | Estimates (x,y) position and updates Blynk pins  | X: V6, Y: V7 |
| tracker.py   | Watches V9 to pause RSSI collection              | Trigger: V9 |

## Auto-Target Mode (Blynk Cloud V201)

Anchor1 can now fetch an angle from Blynk Cloud every 1 second using the REST API:

GET https://blynk.cloud/external/api/get?token=YOUR_TOKEN&V201

This value controls the PAN servo automatically.

### Mode Switching
- Type `AUTO` in Serial → enables auto mode.
- Type `MANUAL` → returns control to the user.
- Manual commands are ignored while in AUTO.

### Value Mapping
Whatever is stored in V201:
raw_value → pan_angle = raw_value * 2  (constrained 0–180)

This lets the dashboard or other anchors push guidance angles.


### How the Servo Control Works
1. Anchor1 powers up, attaches both servos, initializes them at their starting positions, and starts its normal RSSI scan cycle.
2. The serial monitor (115200 baud) becomes the control console for moving the servos.
3. Supported command formats:
   - `PAN <angle>`
   - `TILT <angle>`
   - `(PAN, TILT) = (x, y)` for simultaneous updates.
4. Angle values are constrained between 0 and 180 degrees virtually, and 0 to 90 degrees physically.
5. The servos stay where you last set them. There’s no auto-reset or auto-centering.
6. While you issue servo commands, anchor1 continues scanning WiFi, detecting `tagESP`, and sending RSSI values to Blynk.
7. The servo system now supports two modes:
   - MANUAL (default): user sends commands over Serial.
   - AUTO: the device polls Blynk Cloud V201 every 1 sec and moves automatically.


## Prerequisites (Windows)
1. Python 3.8and installed.
2. Virtual environment and packages:
   ```
   python -m venv .venv
   .venv\Scripts\activate
   pip install --upgrade pip
   pip install requests numpy pyserial
   ```
3. Arduino IDE to flash ESP8266 sketches.
4. Blynk project matching the template/token used across all ESP nodes.

## Configuration
Update constants in all components:
- `BLYNK_AUTH`
- `WIFI_SSID` / `WIFI_PASS`
- `TARGET_SSID` (`tagESP`)
- RSSI virtual pins V0–V3
- `tracker.py` variables: SERIAL_PORT, SERIAL_BAUD, SAMPLES, INTERVAL, and path to fingerprint_data.json

## Calibration
1. Fix all anchors in place.
2. Power the tag (`tagESP`) and place it at calibration grid points.
3. Confirm anchors report live RSSI values to Blynk.
4. Run:
   ```
   python calibration.py
   ```
5. For each prompted grid location, press Enter to sample RSSI. The script averages multiple readings and stores them.

### Fingerprint JSON Format
```json
{
    "1.33_2.33": {
        "V0": -100,
        "V1": -32,
        "V2": 0,
        "V3": 0
    },
    "0.5_1.5": {
        "V0": -42,
        "V1": -38,
        "V2": -5,
        "V3": -7
    }
}
```

## Running the Tracker
1. Confirm fingerprint_data.json exists.
2. Adjust tracker configuration.
3. Launch:
   ```
   python tracker.py
   ```

### Behavior
- Samples RSSI from anchors.
- Estimates position using nearest-neighbor matching.
- Updates Blynk V6/V7.
- If V9 = 1: pauses RSSI sampling and streams serial logs from anchor3.

## Using the Dashboard
- **Lock On:** triggers a visual cutscene.
- **Track:** refreshes coordinates and distance.
- **Launch:** triggers the actuator on V41.
- **RSSI Nodes:** queries V0–V3 every 2 seconds.

## Sniffing & Trilateration
1. Trilateration uses RSSI fingerprints to match to the closest stored signature.
2. Sniffing mode lets anchor3 connect to the tag's soft‑AP and request its HTTP payload.

## Ethics & Safety
> Only use sniffing or scanning features on devices you own or systems where you have explicit permission.

## Dev Notes
> - RSSI averaging timing depends on SAMPLES x INTERVAL.
> - Sniffing requires anchor3 to be physically connected to your PC so tracker.py can read its serial output.
> - Anchors must remain fixed for consistent results.
> - When Auto mode is enabled, anchor1 ignores Serial commands and only listens to the V201 angle. If no value exists in V201, the servo holds its last position.
> - OTA (Using BlynkEdgent) is supported by all nodes.
