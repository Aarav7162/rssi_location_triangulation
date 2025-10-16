# Indoor Position Tracking Using ESP8266/ESP32 and Blynk

![Project Banner](https://via.placeholder.com/800x200.png?text=Indoor+Position+Tracking)

## Table of Contents
1. Project Overview
2. System Architecture
3. Hardware Requirements
4. Software Requirements
5. Setup Instructions
6. Calibration Procedure
7. Live Tracking
8. Python Tracker Code
9. Fingerprint JSON Structure
10. Servo Integration and Lego Turret
11. Data Flow and Communication
12. Future Improvements
13. License

---

## Project Overview

This project demonstrates an indoor tracking system that uses ESP8266 and ESP32 microcontrollers to locate a moving Wi-Fi or BLE-enabled tag within a confined space. The system measures the received signal strength indicator from multiple nodes, compares these readings against pre-recorded fingerprints, and estimates the tag's position in 2D space. Additionally, the angle from the origin is calculated to allow for future integration with servo motors or Lego turrets for visual tracking.

The main objectives are:

- To measure RSSI from multiple ESP nodes.
- To calibrate a room with a fingerprint database of RSSI values at known coordinates.
- To estimate the tag's position in real-time using Python.
- To calculate the angle from a fixed origin point.
- To provide a placeholder for servo or motor integration.

---

## System Architecture

```
+---------------------+        +------------------+
|    Tag Device       |        |    ESP Node 0    |
| Wi-Fi / BLE Tag     |<------>| Measures RSSI    |
+---------------------+        +------------------+
                                     |
                                     v
                             +------------------+
                             | Blynk Cloud API  |
                             +------------------+
                                     |
                                     v
                             +------------------+
                             | Python Tracker   |
                             | - Reads JSON     |
                             | - Computes position |
                             | - Calculates angle |
                             +------------------+
                                     |
                                     v
                            +-------------------+
                            | Servo or Lego Turret |
                            | Placeholder       |
                            +-------------------+
```

Each ESP node continuously measures the RSSI of the tag and sends it to Blynk Cloud. The Python tracker collects these readings, compares them to the fingerprint database, computes the estimated position, and calculates the angle from the origin.

---

## Hardware Requirements

- Four ESP8266 or ESP32 nodes placed in known positions around the tracking area.
- One Wi-Fi or BLE-enabled tag that will be tracked.
- Optional ultrasonic sensor for distance visualization.
- Optional servo motor or Lego turret for angle visualization.
- USB cables, breadboards, and power supplies for ESP nodes.

---

## Software Requirements

- Python 3.11 or higher
- Required Python packages: requests, json, math, statistics
- Arduino IDE for programming the ESP nodes
- Blynk account for sending and receiving data

---

## Setup Instructions

### ESP Node Setup

1. Program each ESP node using the Arduino IDE to measure RSSI from the tag.
2. Connect each ESP node to Wi-Fi and configure it to send RSSI readings to specific Blynk virtual pins (V0 to V3).
3. The nodes should transmit RSSI at regular intervals, e.g., every 0.5 seconds.

### Python Environment Setup

1. Clone this repository:

```bash
git clone https://github.com/<your-username>/ESP-RSSI-Tracking.git
cd ESP-RSSI-Tracking
```

2. Install the Python dependencies:

```bash
pip install requests
```

3. Ensure that `fingerprint_data.json` is present in the same folder as the tracker script.

---

## Calibration Procedure

1. Run `calibration.py` to record RSSI fingerprints for the room.
2. The script will prompt the user to enter X and Y coordinates for each calibration point.
3. For each calibration point:
   - Press Enter to start the RSSI measurement.
   - The script will collect a predefined number of samples (e.g., 20) and calculate the average RSSI for each node.
   - These averages will be stored in `fingerprint_data.json` as a key-value pair, where the key is the coordinate and the value is the average RSSI per node.
4. Repeat this for all desired points in the room. For example, 200 calibration points evenly distributed will improve accuracy.

---

## Live Tracking

Run the Python tracker using the following command:

```bash
python tracker.py
```

The console output will continuously display:

- Current average RSSI from each node.
- Estimated position in meters.
- Angle from the origin.
- Placeholder message for servo or turret rotation.

Example output:

```
🔧 Live RSSI Tracking Started...
RSSI: {'V0': -35, 'V1': -32, 'V2': 0, 'V3': 0}
→ Estimated Position: (1.33, 2.33) meters
→ Angle from origin: 60.35°
[SERVO PLACEHOLDER] Would rotate to 60.35°
```

Tracking continues indefinitely until interrupted with Ctrl+C.

---

## Python Tracker Code

- Collects a specified number of RSSI samples per cycle.
- Averages the readings for each node.
- Finds the closest fingerprint point using Euclidean distance in RSSI space.
- Calculates the angle from the origin using `atan2`.
- Prints results to the console.
- Placeholder code is provided for servo or Lego turret rotation.

The script can be modified to include live per-sample logging for more granular monitoring.

---

## Fingerprint JSON Structure

The JSON file stores calibration points as follows:

```json
{
    "1.33_2.33": {"V0": -35, "V1": -32, "V2": 0, "V3": 0},
    "0.5_1.5": {"V0": -42, "V1": -38, "V2": -5, "V3": -7}
}
```

- Keys are the X_Y coordinates in meters.
- Values are dictionaries of average RSSI per node.

---

## Servo Integration and Lego Turret

- Currently represented as a placeholder in the Python tracker.
- The `angle` calculated can be sent to a servo or Lego turret to rotate in real-time.
- Ultrasonic sensors can be added for precise distance measurement.
- Future versions can include automatic aiming and visualization of tracked positions.

---

## Data Flow and Communication

1. The tag emits a Wi-Fi or BLE signal.
2. Each ESP node measures the RSSI from the tag.
3. ESP nodes send RSSI readings to Blynk Cloud via API.
4. The Python tracker pulls RSSI values from Blynk, averages them, and compares them with calibration fingerprints.
5. Estimated position and angle are printed and optionally used for servo/turret control.

---

## Future Improvements

- Real-time GUI showing tag position on a room map.
- Automatic calibration with a robot moving to known points.
- Integration with BLE 5.1 AoA hardware for higher precision.
- Smoothing and prediction algorithms to reduce noise in position estimates.
- Multi-floor tracking and 3D position estimation.

---

## License

MIT License. The project can be modified and used for personal, educational, or experimental purposes.

