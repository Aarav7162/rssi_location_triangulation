# WiFi RSSI Monitoring Framework – Week 1

## Project Overview
This project is the foundation for an IoT-based Wi-Fi monitoring and location detection system. For Week 1, we’re setting up the basic infrastructure using two ESP8266 boards and the Blynk dashboard.  

- **ESP1** and **ESP2** measure the Wi-Fi signal strength (RSSI) of a target SSID (`SomeGuyWithPretzels`) and send these values to the Blynk dashboard.  
- An **LED** connected to ESP1 can be controlled remotely from the Blynk dashboard via a virtual pin (V2 or a button widget).  

This setup is designed as the groundwork for more advanced features in future weeks, like distance estimation and triangulation.

---

## Hardware Requirements
- 2 × ESP8266 boards (NodeMCU, Wemos D1 Mini, etc.)  
- 1 × LED  
- Jumper wires  
- Breadboard (optional for easy connections)  

---

## Wiring Details

### ESP1 – LED Control + RSSI
| Pin | Connection | Purpose |
|-----|------------|---------|
| D3  | LED (+)    | Status indicator / Tracking confirmation |
| GND | LED (−)    | Ground connection |
| –   | Internal Wi-Fi scan | RSSI measurement |

### ESP2 – RSSI Only
- No additional wiring; ESP2 only scans for Wi-Fi RSSI.

---

## Software Requirements
- [Arduino IDE](https://www.arduino.cc/en/software)  
- ESP8266 Board Package ([Installation Guide](https://arduino.esp8266.com/stable/package_esp8266com_index.json))  
- [Blynk Library](https://github.com/blynkkk/blynk-library)  
- Blynk App (iOS / Android) configured with the following virtual pins:  
  - **V0** → ESP1 RSSI  
  - **V1** → ESP2 RSSI  
  - **V7** → LED control  

---

## Blynk Dashboard Setup
1. Open the Blynk app and create a new project for **ESP8266**.  
2. Add widgets for RSSI display and LED control. Assign the correct virtual pins:  
   - **V0**: Display RSSI from ESP1  
   - **V1**: Display RSSI from ESP2  
   - **V7**: LED control (switch or button widget)  

> Make sure your dashboard layout is intuitive for monitoring live signal strength.

---

## Usage Instructions
1. Open the Arduino IDE and modify both ESP sketches with your specific network details:
   ```cpp
   const char* ssid = "ssid";
   const char* pass = "pass";
   char auth[] = "auth";
2. Upload esp1.ino to ESP1 and esp2.ino to ESP2.
3. Open the Serial Monitor to check connection status and live RSSI values.
4. Access the Blynk dashboard to monitor live RSSI readings and control the LED.

## Next Steps
- Replace placeholders with the actual target SSID.
- Calculate approximate distances using RSSI values.
- Implement basic 2D triangulation using ESP1, ESP2, and a computer or third ESP node.
- Trigger alerts or events when signal strength exceeds or drops below set thresholds.
- Expand the system to multiple Wi-Fi networks or multiple ESP nodes for advanced location mapping.

## Notes
> This framework only provides raw RSSI readings; no distance calculation or triangulation is implemented yet.
> Make sure both ESP modules are connected to the same Wi-Fi network for accurate monitoring.
