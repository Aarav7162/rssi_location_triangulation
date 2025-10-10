# WiFi RSSI Monitoring Framework

## Project Overview
This is the Week 1 framework for the IoT project. The goal is to set up a working IoT infrastructure using two ESP8266 boards and the Blynk dashboard.  

- ESP1 measures the RSSI of a specific Wi-Fi SSID (`SomeGuyWithPretzels (Replace with your target network)`) and sends the value to Blynk Virtual Pin V0.
- ESP2 measures the same RSSI and sends it to Blynk Virtual Pin V1.
- An LED connected to ESP1 can be controlled from the Blynk dashboard via V2 or a button widget.

This framework is made for further functionality like triangulation and location detection in later weeks.

---

## Hardware Requirements
- 2 × ESP8266 (NodeMCU, Wemos D1 mini, etc.)
- LED
- Jumper wires
- Breadboard (optional)

---

## Wiring

**ESP1 – LED Control**
- D3 → LED (+) for Tracking Conformation
- GND → LED (−)
- RSSI Wi-Fi Scanning

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
2. Add the widgets attached under /extras and change datastreams according to your preferences.: 

---

## Usage

1. Replace the following placeholders in both ESP codes:
   - `YOUR_WIFI_SSID` → Your Wi-Fi SSID
   - `YOUR_WIFI_PASSWORD` → Your Wi-Fi password
   - `YOUR_AUTH_TOKEN` → Blynk project auth token

2. Upload `esp1.ino` to ESP1 and `esp2.ino` to ESP2.  
3. Open the Serial Monitor to check connection and RSSI values.  
4. Open the Blynk dashboard to view live RSSI and control the LED.  

---

## Next Step
- Replace placeholders with actual RSSI of the SSID `SomeGuyWithPretzels`.  
- Implement distance estimation using RSSI.  
- Add triangulation using ESP1, ESP2, and a computer for 2D triangulation.  
- Add events or alerts if RSSI exceeds thresholds.

---

## Notes
> No triangulation or advanced calculations are included yet.  

---

