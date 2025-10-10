# WiFi RSSI Monitoring Framework

## Project Overview
This is the Week 1 framework for the IoT project. The goal is to set up a working IoT infrastructure using two ESP8266 boards and the Blynk dashboard.  

- ESP1 measures the RSSI of a specific Wi-Fi SSID (`SomeGuyWithPretzels`) and sends the value to Blynk Virtual Pin V0.
- ESP2 measures the same RSSI and sends it to Blynk Virtual Pin V1.
- An LED connected to ESP1 can be controlled from the Blynk dashboard via V2 or a button widget.

This framework establishes Wi-Fi connectivity, Blynk integration, multi-device setup, and dashboard visualization, ready for further functionality like triangulation or speed measurement in later weeks.

---

## Hardware Requirements
- 2 × ESP8266 (NodeMCU, Wemos D1 mini, etc.)
- 1 × LED + resistor (220Ω)
- Jumper wires
- Breadboard (optional)

---

## Wiring

**ESP1 – LED Control**
- D3 → LED (+) for Tracking Conformation
- GND → LED (−)

**ESP2 – No additional wiring**  
- ESP2 only scans for Wi-Fi RSSI.

---

## Software Requirements
- [Arduino IDE](https://www.arduino.cc/en/software)
- [ESP8266 Board Package](https://arduino.esp8266.com/stable/package_esp8266com_index.json)
- [Blynk Library](https://github.com/blynkkk/blynk-library)
- Blynk App (iOS / Android) configured with a project using:
  - V0 → ESP1 RSSI  
  - V1 → ESP2 RSSI  
  - V2 → LED control

---

## Blynk Dashboard Setup

1. Create a **new Blynk project** with **ESP8266**.  
2. Add the following **widgets**:
   - **Label or Gauge** → V0 (ESP1 RSSI)  
   - **Label or Gauge** → V1 (ESP2 RSSI)  
   - **Button** → V2 (ESP1 LED)  
3. Set button mode to **Switch**.

---

## Code Structure

WiFi-RSSI-Framework/
├── esp1/
│ └── esp1.ino # LED control + RSSI placeholder
├── esp2/
│ └── esp2.ino # RSSI placeholder
└── README.md


- `esp1.ino`: Connects ESP1 to Wi-Fi and Blynk, controls LED, sends RSSI to V0.  
- `esp2.ino`: Connects ESP2 to Wi-Fi and Blynk, sends RSSI to V1.  
- Both ESPs are fully **multi-device ready**.

---

## Usage

1. Replace the following placeholders in both ESP codes:
   - `YOUR_WIFI_SSID` → Your Wi-Fi SSID
   - `YOUR_WIFI_PASSWORD` → Your Wi-Fi password
   - `YOUR_AUTH_TOKEN` → Blynk project auth token

2. Upload `esp1.ino` to ESP1 and `esp2.ino` to ESP2.  
3. Open the **Serial Monitor** to check connection and RSSI values.  
4. Open the **Blynk dashboard** to view live RSSI and control the LED.  

---

## What’s Next (Week 2+)
- Replace placeholders with **actual RSSI of the SSID `SomeGuyWithPretzels`**.  
- Implement **distance estimation** using RSSI.  
- Add **triangulation** using ESP1, ESP2, and optionally your computer.  
- Add **events or alerts** if RSSI exceeds thresholds.

---

## Notes
> This project demonstrates **Week 1 framework**, focusing on **infrastructure, dashboard setup, and multi-device connectivity**.  
> No triangulation or advanced calculations are included yet.  

---

