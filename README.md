# RSSI Location Triangulation — Folder: two

# Overview
This folder contains a complete indoor localization/fingerprinting system built using **ESP8266 anchors**, a **soft‑AP tag** (`tagESP`), and Python utilities. The system is designed for estimating the (x,y) position of a mobile device using RSSI (Received Signal Strength Indicator) fingerprinting. It uses multiple fixed anchors to trilaterate the tag device's position (via 1‑NN matching on RSSI fingerprints) and — when triggered — can sniff and stream the tag's HTTP requests (the tag runs a tiny HTTP server at http://192.168.4.56/), allowing you to capture short payloads served by the tag.

It is intended for educational purposes, proof-of-concept IoT localization, and controlled indoor tracking experiments.

## Folder Structure
```
two/
├── anchor1.ino          # ESP8266 sketch reporting RSSI to Blynk V0
├── anchor2.ino          # ESP8266 sketch reporting RSSI to Blynk V1
├── anchor3.ino          # ESP8266 sketch reporting RSSI to Blynk V2, handles sniffing triggered by V9, returns payload to V10
├── anchor4.ino          # ESP8266 sketch reporting RSSI to Blynk V3
├── tag_esp.ino          # ESP soft-AP serving SSID `tagESP` with simple HTTP payload
├── calibration.py       # Python utility to collect RSSI fingerprints at known grid points
├── tracker.py           # Python live tracker: estimates position and updates Blynk V6/V7, handles serial streaming when V9 = 1
├── fingerprint_data.json# JSON file storing collected RSSI fingerprints
└── README.md            # Documentation (this file)
```

## Quick Mapping
| Device       | Function                                         | Blynk Pin |
|--------------|-------------------------------------------------|-----------|
| anchor1.ino  | Reports RSSI to first anchor                     | V0        |
| anchor2.ino  | Reports RSSI to second anchor                    | V1        |
| anchor3.ino  | Reports RSSI, handles sniffing                  | V2 / V10  |
| anchor4.ino  | Reports RSSI to fourth anchor                    | V3        |
| tracker.py   | Estimates (x,y) position and updates Blynk pins | X: V6, Y: V7 |
| tracker.py   | Watches V9 to pause RSSI collection             | Trigger: V9 |

## Prerequisites (Windows)
1. Python 3.8+ installed.
2. Virtual environment (recommended) and packages:
   ```
   python -m venv .venv
   .venv\Scripts\activate
   pip install --upgrade pip
   pip install requests numpy pyserial
   ```
3. Arduino IDE or PlatformIO to flash ESP8266 sketches.
4. Blynk account with project matching the template/token in the ESP sketches.

## Configuration
Before running/flashing, update constants in the scripts:
- **BLYNK_AUTH** — must match across all ESP anchors and Python scripts.
- **WIFI_SSID / WIFI_PASS** — your home WiFi credentials for anchors.
- **TARGET_SSID** — `tagESP`, the SSID that anchors scan for.
- **RSSI virtual pins** — ensure anchors map to V0..V3 correctly.
- **tracker.py**:
  - `SERIAL_PORT` (COM port where anchor3 is connected)
  - `SERIAL_BAUD` (default 9600)
  - `SAMPLES` and `INTERVAL` for RSSI averaging
  - `CALIBRATION_FILE` path to your fingerprint_data.json

## Calibration
1. Place anchors at fixed locations; do not move them.
2. Boot the tag (soft-AP) and move it to the grid points you want to calibrate.
3. Start Blynk and verify anchors report RSSI.
4. Activate the Python virtual environment and run:
   ```
   python calibration.py
   ```
5. Follow prompts:
   - Move to indicated (x,y) point.
   - Press Enter to start RSSI collection.
   - Script samples SAMPLES_PER_POINT RSSI readings for each anchor.
   - Averages the readings and appends to fingerprint_data.json.
6. Resume calibration if interrupted; fingerprint_data.json updates incrementally.

### Fingerprint JSON Format
Keys are "x_y" strings. Values are RSSI readings per anchor.
```json
{
  "0.5_1.5": { "V0": -42, "V1": -38, "V2": -5, "V3": -7 },
  "1.33_2.33": { "V0": -100, "V1": -32, "V2": 0, "V3": 0 }
}
```
- Zero or missing readings may indicate failed scans.
- Always calibrate with anchors in fixed positions.

## Running the Tracker
1. Ensure fingerprint_data.json exists.
2. Edit tracker.py constants as needed.
3. Run:
   ```
   python tracker.py
   ```

### Behavior
- Loops indefinitely performing RSSI averaging and 1-NN nearest neighbor position estimation.
- Updates Blynk virtual pins: X → V6, Y → V7.
- If Blynk V9 == 1:
  - RSSI collection pauses.
  - Tracker reads serial from `SERIAL_PORT` (anchor3 ESP) and prints to console.
  - Once V9 returns to 0, RSSI tracking resumes.

### Example Console Output
```
🔧 Live RSSI Tracking Started...

[RSSI AVG] {'V0': -100, 'V1': -100, 'V2': -61, 'V3': -69}
[POSITION] → Estimated Position: (1.33, 2.33) meters
[BLYNK] Coordinates sent: (1.33, 2.33)
[PAUSE] V9 = 1 → Reading ESP Serial...
[SERIAL] Reading from COM5...
[ESP] Trying password: imthepassword
[ESP] Connected to tagESP
[PAUSE] V9 released → Resuming RSSI tracking...
```

## Notes
- Tracker does **not stop automatically**. Modify main loop for bounded iterations if needed.
- RSSI collection timing: `SAMPLES × INTERVAL` seconds per cycle.
- Serial reading shows live ESP debug logs when V9 = 1.
- Ensure COM port is correct; pyserial requires exclusive access.

## Optional: Limit Tracker Cycles
Replace infinite loop with bounded loop in `tracker.py`:
```python
MAX_CYCLES = 20
for cycle in range(MAX_CYCLES):
    ...  # RSSI collection and estimation
    print(f"[CYCLE] {cycle+1}/{MAX_CYCLES}")
print("[STOP] Reached MAX_CYCLES.")
```
This allows testing or short runs without stopping manually.

## Troubleshooting
- **No serial output:** Check COM port and ensure ESP is connected and powered.
- **Zero RSSI readings:** Ensure anchors are online and Blynk is reporting correctly.
- **Python exceptions:** Verify packages `requests`, `numpy`, `pyserial` are installed in the activated virtualenv.
- **ESP connection issues:** Anchor3 must be flashed correctly and serial monitor closed in Arduino IDE when running Python serial read.

## Summary
This system provides a complete indoor RSSI-based positioning pipeline:
- **Calibration** → Build fingerprint database
- **Anchors** → Scan RSSI, send to Blynk
- **Tracker** → Estimate position, update Blynk, handle live serial logging on V9 trigger

It’s designed for experimentation, debugging, and learning about IoT localization, BLE/WiFi RSSI-based tracking, and Python + ESP8266 integration.

## Sniffing & Trilateration — what this actually does
This project does two related things:

1. **Trilateration (fingerprinting-based):** the tracker uses RSSI samples from multiple fixed ESP8266 anchors (V0..V3) and compares the measured RSSI vector to a calibrated fingerprint database. The nearest fingerprint (1‑NN on Euclidean distance in RSSI space) is used as the estimated (x,y) position for the tag device.

2. **HTTP sniffing of the tag:** anchor3 (the special anchor) can switch into a sniffing routine when triggered (Blynk V9). In sniff mode it connects to the soft‑AP tag (`tagESP`) and requests `http://192.168.4.56/` repeatedly to capture the short text payloads served by the tag. The captured payloads are returned to the Blynk app (V10) and — when tracker.py detects V9 == 1 — the Python script opens a serial connection to anchor3 and streams whatever the anchor prints (typically the sniffed HTTP payloads).

### What I updated to support this (developer notes)
- **Anchor3 sketch** must implement an explicit sniffing routine that:
  - Listens for V9 going HIGH/1 via Blynk and reports its state to Serial.
  - Attempts to connect to the tag `tagESP` (using configured candidate passwords if applicable), performs an HTTP GET on `http://192.168.4.56/`, and prints or Blynk-writes the payload.
  - Minimizes repeated noisy prints — print a single status line per attempt and only print HTTP payloads when they change or on new events.

- **Tracker.py** now:
  - Polls Blynk V9 continuously and **immediately** pauses RSSI sampling when V9 == 1.
  - Opens the configured serial port (SERIAL_PORT) and streams anchor3's Serial output live to the console while V9 remains 1.
  - When V9 returns to 0 the script closes the serial port and resumes normal RSSI sampling and position updates to V6/V7.

- **calibration.py** and fingerprint format unchanged, but calibration should avoid points with 0 or missing RSSI values because they degrade 1‑NN accuracy.

### Configuration checklist for sniffing to work
- Anchor3 must be flashed with the `anchor3.ino` sketch that supports sniffing and serial debug output.
- Anchor3 must be physically connected to the PC (or otherwise expose a serial endpoint) at the configured `SERIAL_PORT`.
- Blynk V9 must be wired to a dashboard button/widget configured as a persistent **Switch** (so it remains 1 while pressed) or otherwise set programmatically to 1 while you want sniffing.
- The tag (`tagESP`) must be running and reachable at `http://192.168.4.56/` when anchor3 attempts the HTTP GET. Confirm via a browser from a machine on the same network if necessary.
- If anchor3 uses WiFi password guessing to join the tag's AP, limit retries and ensure anchor3 prints minimal status to avoid terminal spam.

### Output examples (sniffing)
When V9 == 1 you should see console output similar to:
```
[PAUSE] V9 = 1 → Pausing RSSI tracking, reading ESP Serial...
[SERIAL] Reading from COM5...
[ESP] Trying password: imthepassword
[ESP] Connected to tagESP (IP 192.168.4.56)
[ESP] HTTP payload: hello-from-tag
[ESP] HTTP payload: sensor=24;state=ok
[PAUSE] V9 released → Resuming RSSI tracking...
```

### Ethics & safety note
Sniffing HTTP requests or attempting to connect to WiFi networks should only be done on devices and networks you own or have explicit permission to test. This project is intended for controlled lab experiments and educational use—do not use it to access or intercept other people's devices or networks without consent.

If you want, I can also: 
- Add a concise `anchor3.ino` sniffing checklist/snippet to this README. 
- Provide a tight `anchor3.ino` sketch that prints a single status line per connection attempt and only emits HTTP payloads when new.

---



